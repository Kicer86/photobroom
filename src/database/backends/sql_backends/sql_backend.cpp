/*
 * Base for SQL-based backends
 * This class is meant to be included to each project using it.
 *
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "sql_backend.hpp"

#include <iostream>
#include <set>
#include <thread>
#include <chrono>
#include <sstream>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QPixmap>
#include <QBuffer>
#include <QString>

#include <OpenLibrary/utils/optional.hpp>

#include <core/tag.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <database/filter.hpp>
#include <database/iphoto_info_manager.hpp>
#include <database/project_info.hpp>
#include <database/backends/photo_info.hpp>

#include "table_definition.hpp"
#include "sql_query_constructor.hpp"
#include "tables.hpp"
#include "query_structs.hpp"
#include "sql_select_query_generator.hpp"
#include "variant_converter.hpp"


namespace Database
{
    namespace
    {
        QByteArray toPrintable(const QPixmap& pixmap)
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);

            pixmap.save(&buffer, "JPEG");

            return bytes.toBase64();
        }

        QPixmap fromPrintable(const QByteArray& data)
        {
            const QByteArray bytes = QByteArray::fromBase64(data);
            QPixmap pixmap;

            const bool status = pixmap.loadFromData(bytes, "JPEG");

            if (status == false)
            {
                //TODO: load thumbnail from file
            }

            return pixmap;
        }


        struct Transaction
        {
            Transaction(): m_level(0), m_name(), m_logger(nullptr) {}

            Transaction(const Transaction &) = delete;
            Transaction& operator=(const Transaction &) = delete;

            void setDBName(const QString& name)
            {
                m_name = name;
            }
            
            void set(ILogger* logger)
            {
                m_logger = logger;
            }

            BackendStatus begin()
            {
                assert(m_name != "");
                BackendStatus status(StatusCodes::Ok);

                if (m_level++ == 0)
                {
                    QSqlDatabase db = QSqlDatabase::database(m_name, false);

                    if (db.isOpen())
                        status = db.transaction()? StatusCodes::Ok: StatusCodes::TransactionFailed;
                }

                return status;
            }


            BackendStatus end()
            {
                assert(m_name != "" && m_level > 0);
                BackendStatus status = StatusCodes::Ok;

                if (--m_level == 0)
                {
                    const auto start = std::chrono::steady_clock::now();

                    QSqlDatabase db = QSqlDatabase::database(m_name, false);

                    if (db.isOpen())
                    {
                        status = db.commit()? StatusCodes::Ok: StatusCodes::TransactionCommitFailed;

                        if (status == false)
                            db.rollback();
                    }

                    const auto end_t = std::chrono::steady_clock::now();
                    const auto diff = end_t - start;
                    const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
                    
                    std::stringstream logInfo;
                    logInfo << "Transaction commit: " << diff_ms << "ms";
                    m_logger->log(ILogger::Severity::Debug, logInfo.str());
                }

                return status;
            }

            int m_level;
            QString m_name;
            ILogger* m_logger;
        };

    }

    struct StorePhoto;

    /*****************************************************************************/


    struct ASqlBackend::Data
    {
            ASqlBackend* m_backend;
            std::thread::id m_database_thread_id;
            QString m_connectionName;
            IPhotoInfoCache* m_photoInfoCache;
            Transaction m_transaction;
            std::unique_ptr<ILogger> m_logger;
            std::set<IBackend::IEvents *> m_observers;
            bool m_dbHasSizeFeature;
            bool m_dbOpen;

            Data(ASqlBackend* backend);
            ~Data();
            Data(const Data &) = delete;
            Data& operator=(const Data &) = delete;

            BackendStatus exec(const QString& query, QSqlQuery* result) const;
            BackendStatus exec(const SqlQuery& query, QSqlQuery* result) const;
            ol::Optional<unsigned int> store(const TagNameInfo& nameInfo) const;
            bool store(const TagValue& value, int photo_id, int tag_id) const;
            bool store(const IPhotoInfo::Ptr& data);
            IPhotoInfo::Ptr getPhoto(const IPhotoInfo::Id &);
            std::deque<TagNameInfo> listTags() const;
            std::deque<QVariant> listTagValues(const TagNameInfo& tagName);
            std::deque<QVariant> listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &);
            IPhotoInfo::List getPhotos(const std::deque<IFilter::Ptr> &);
            int getPhotosCount(const std::deque<IFilter::Ptr> &);

            void postPhotoInfoCreation(const IPhotoInfo::Ptr &) const;

        private:
            ol::Optional<unsigned int> findTagByName(const QString& name) const;
            QString generateFilterQuery(const std::deque<IFilter::Ptr>& filter);

            bool storeThumbnail(int photo_id, const QPixmap &) const;
            bool storeSha256(int photo_id, const IPhotoInfo::Sha256sum &) const;
            bool storeTags(int photo_id, const Tag::TagsList &) const;
            bool storeFlags(int photo_id, const IPhotoInfo::Ptr &) const;

            Tag::TagsList getTagsFor(const IPhotoInfo::Id &);
            ol::Optional<QPixmap> getThumbnailFor(const IPhotoInfo::Id &);
            ol::Optional<IPhotoInfo::Sha256sum> getSha256For(const IPhotoInfo::Id &);
            void updateFlagsOn(IPhotoInfo*, const IPhotoInfo::Id &);
            QString getPathFor(const IPhotoInfo::Id &);
            IPhotoInfo::List fetch(QSqlQuery &);
    };


    ASqlBackend::Data::Data(ASqlBackend* backend): m_backend(backend),
                                                   m_database_thread_id(),
                                                   m_connectionName(""),
                                                   m_photoInfoCache(nullptr),
                                                   m_transaction(),
                                                   m_logger(nullptr),
                                                   m_observers(),
                                                   m_dbHasSizeFeature(false),
                                                   m_dbOpen(false)
    {

    }


    ASqlBackend::Data::~Data()
    {

    }


    BackendStatus ASqlBackend::Data::exec(const QString& query, QSqlQuery* result) const
    {
        // threads cannot be used with sql connections:
        // http://qt-project.org/doc/qt-5/threads-modules.html#threads-and-the-sql-module
        // make sure the same thread is used as at construction time.
        assert(std::this_thread::get_id() == m_database_thread_id);

        std::string logMessage = query.toStdString();

        const auto start = std::chrono::steady_clock::now();
        const BackendStatus status = result->exec(query)? StatusCodes::Ok: StatusCodes::QueryFailed;
        const auto end = std::chrono::steady_clock::now();
        const auto diff = end - start;
        const auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        logMessage = logMessage + " Exec time: " + std::to_string(diff_ms) + "ms";

        m_logger->log(ILogger::Severity::Debug, logMessage);

        if (status == false)
            m_logger->log(ILogger::Severity::Error,
                          "Error: " + result->lastError().text().toStdString() + " while performing query: " + query.toStdString());

        assert(status);
        return status;
    }


    BackendStatus ASqlBackend::Data::exec(const SqlQuery& query, QSqlQuery* result) const
    {
        auto& queries = query.getQueries();
        BackendStatus status(StatusCodes::Ok);

        for(size_t i = 0; i < queries.size() && status; i++)
            status = exec(queries[i], result);

        return status;
    }


    ol::Optional<unsigned int> ASqlBackend::Data::store(const TagNameInfo& nameInfo) const
    {
        const QString& name = nameInfo.getName();
        const int type = nameInfo.getType();
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        //check if tag exists
        ol::Optional<unsigned int> tagId = findTagByName(name);

        if (! tagId)  //tag not yet in database
        {
            const QString queryStr = QString("INSERT INTO %1 (id, name, type) VALUES (NULL, '%2', '%3');")
                                     .arg(TAB_TAG_NAMES)
                                     .arg(name)
                                     .arg(type);

            const bool status = exec(queryStr, &query);

            if (status)
                tagId = query.lastInsertId().toUInt();
        }

        return tagId;
    }


    bool ASqlBackend::Data::store(const TagValue& value, int photo_id, int tag_id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        //store tag values
        const VariantConverter convert;
        const QString tag_value = convert(value.get());

        InsertQueryData queryData(TAB_TAGS);
        queryData.setColumns("id", "value", "photo_id", "name_id");
        queryData.setValues(InsertQueryData::Value::Null,
                            tag_value,
                            photo_id,
                            tag_id);

        auto query_str = m_backend->getQueryConstructor()->insertOrUpdate(queryData);

        const bool status = exec(query_str, &query);

        return status;
    }


    std::deque<TagNameInfo> ASqlBackend::Data::listTags() const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        const QString query_str("SELECT name, type FROM " TAB_TAG_NAMES ";");

        const bool status = exec(query_str, &query);
        std::deque<TagNameInfo> result;

        while (status && query.next())
        {
            const QString name       = query.value(0).toString();
            const unsigned int value = query.value(1).toUInt();

            TagNameInfo tagName(name, value);
            result.push_back(tagName);
        }

        return result;
    }


    std::deque<QVariant> ASqlBackend::Data::listTagValues(const TagNameInfo& tagName)
    {
        const ol::Optional<unsigned int> tagId = findTagByName(tagName);

        std::deque<QVariant> result;

        if (tagId)
        {
            VariantConverter convert;
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            QSqlQuery query(db);
            const QString query_str = QString("SELECT value FROM " TAB_TAGS " WHERE name_id=\"%1\";")
                                      .arg(*tagId);

            const bool status = exec(query_str, &query);

            while (status && query.next())
            {
                const QString raw_value = query.value(0).toString();
                const QVariant value = convert(tagName.getType(), raw_value);

                result.push_back(value);
            }
        }

        return result;
    }


    std::deque<QVariant> ASqlBackend::Data::listTagValues(const TagNameInfo& tagName, const std::deque<IFilter::Ptr>& filter)
    {
        const QString filterQuery = generateFilterQuery(filter);

        //from filtered photos, get info about tags used there
        QString queryStr = "SELECT DISTINCT %2.value FROM ( %1 ) AS distinct_select JOIN (%2, %3) ON (photos_id=%2.photo_id AND %3.id=%2.name_id) WHERE name='%4'";

        queryStr = queryStr.arg(filterQuery);
        queryStr = queryStr.arg(TAB_TAGS);
        queryStr = queryStr.arg(TAB_TAG_NAMES);
        queryStr = queryStr.arg(tagName.getName());

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        std::deque<QVariant> result;
        const bool status = exec(queryStr, &query);

        if (status)
        {
            VariantConverter convert;

            while (status && query.next())
            {
                const QString raw_value = query.value(0).toString();
                const QVariant value = convert( tagName.getType(), raw_value);

                result.push_back(value);
            }
        }

        return result;
    }


    IPhotoInfo::List ASqlBackend::Data::getPhotos(const std::deque<IFilter::Ptr>& filter)
    {
        const QString queryStr = generateFilterQuery(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        exec(queryStr, &query);
        auto result = fetch(query);

        return result;
    }


    int ASqlBackend::Data::getPhotosCount(const std::deque<IFilter::Ptr>& filter)
    {
        const QString queryStr = generateFilterQuery(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        exec(queryStr, &query);

        int result = 0;

        if (m_dbHasSizeFeature)
            result = query.size();
        else
            result = query.next()? 1: 0;

        return result;
    }


    void ASqlBackend::Data::postPhotoInfoCreation(const IPhotoInfo::Ptr& photoInfo) const
    {
        //introduce to cache
        m_photoInfoCache->introduce(photoInfo);

        //notifications
        for(IBackend::IEvents* observer: m_observers)
            observer->photoInfoConstructed(photoInfo);
    }


    ol::Optional<unsigned int> ASqlBackend::Data::findTagByName(const QString& name) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        const QString find_tag_query = QString("SELECT id FROM " TAB_TAG_NAMES " WHERE name =\"%1\";").arg(name);
        const bool status = exec(find_tag_query, &query);

        ol::Optional<unsigned int> result;

        if (status && query.next())
            result = query.value(0).toInt();

        return result;
    }


    QString ASqlBackend::Data::generateFilterQuery(const std::deque<IFilter::Ptr>& filters)
    {
        const QString result = SqlSelectQueryGenerator().generate(filters);

        return result;
    }


    bool ASqlBackend::Data::storeThumbnail(int photo_id, const QPixmap& pixmap) const
    {
        InsertQueryData data(TAB_THUMBS);

        data.setColumns("photo_id", "data");
        data.setValues(QString::number(photo_id), QString(toPrintable(pixmap)) );

        SqlQuery queryStrs = m_backend->getQueryConstructor()->insertOrUpdate(data);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = exec(queryStrs, &query);

        return status;
    }
    
    
    bool ASqlBackend::Data::storeSha256(int photo_id, const IPhotoInfo::Sha256sum& sha256) const
    {
        InsertQueryData data(TAB_SHA256SUMS);

        data.setColumns("photo_id", "sha256");
        data.setValues(QString::number(photo_id), sha256.c_str());

        SqlQuery queryStrs = m_backend->getQueryConstructor()->insertOrUpdate(data);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = exec(queryStrs, &query);

        return status;
    }


    bool ASqlBackend::Data::storeTags(int photo_id, const Tag::TagsList& tagsList) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = true;

        //remove all tags already attached to photo. TODO: maybe some inteligence here?
        const QString deleteQuery = QString("DELETE FROM " TAB_TAGS " WHERE photo_id=\"%1\"").arg(photo_id);
        status = exec(deleteQuery, &query);

        for (auto it = tagsList.begin(); status && it != tagsList.end(); ++it)
        {
            //store tag name
            const ol::Optional<unsigned int> tag_id = store(it->first);

            if (tag_id)
            {
                const TagValue& value = it->second;
                const int tagid = *tag_id;
                status = store(value, photo_id, tagid);
            }
            else
                status = false;
        }

        return status;
    }


    bool ASqlBackend::Data::storeFlags(int photo_id, const IPhotoInfo::Ptr& photoInfo) const
    {
        InsertQueryData queryData(TAB_FLAGS);
        queryData.setColumns("id", "photo_id", "staging_area", "tags_loaded", "sha256_loaded", "thumbnail_loaded");
        queryData.setValues( InsertQueryData::Value::Null,
                             QString::number(photo_id),
                             photoInfo->getFlag(IPhotoInfo::FlagsE::StagingArea),
                             photoInfo->getFlag(IPhotoInfo::FlagsE::ExifLoaded),
                             photoInfo->getFlag(IPhotoInfo::FlagsE::Sha256Loaded),
                             photoInfo->getFlag(IPhotoInfo::FlagsE::ThumbnailLoaded));

        auto queryStrs = m_backend->getQueryConstructor()->insertOrUpdate(queryData);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = exec(queryStrs, &query);

        return status;
    }


    bool ASqlBackend::Data::store(const IPhotoInfo::Ptr& data)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        bool status = m_transaction.begin();

        //store path and sha256
        IPhotoInfo::Id id = data->getID();
        const bool updating = id.valid();
        const bool inserting = !updating;

        InsertQueryData insertData(TAB_PHOTOS);
        insertData.setColumns("path", "store_date");
        insertData.setValues(data->getPath(), InsertQueryData::Value::CurrentTime);

        SqlQuery queryStrs;

        if (inserting)
        {
            insertData.setColumns("id");
            insertData.setValues(InsertQueryData::Value::Null);
            queryStrs = m_backend->getQueryConstructor()->insert(insertData);
        }
        else
        {
            UpdateQueryData updateData(insertData);
            updateData.setCondition( "id", QString::number(id.value()) );
            queryStrs = m_backend->getQueryConstructor()->update(updateData);
        }

        //execute update/insert
        if (status)
            status = exec(queryStrs, &query);

        //update id
        if (status && inserting)                       //Get Id from database after insert
        {
            QVariant photo_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            status = photo_id.isValid();

            if (status)
                id = IPhotoInfo::Id(photo_id.toInt());
        }

        //make sure id is set
        if (status)
            status = id.valid();

        //store used tags
        Tag::TagsList tags = data->getTags();

        if (status)
            status = storeTags(id, tags);

        if (status && data->isThumbnailLoaded())
            status = storeThumbnail(id, data->getThumbnail());
        
        if (status && data->isSha256Loaded())
            status = storeSha256(id, data->getSha256());

        if (status)
            status = storeFlags(id, data);

        if (status)
            status = m_transaction.end();
        
        //store id in photo
        if (status && inserting)
            data->initID(id);

        return status;
    }


    IPhotoInfo::Ptr ASqlBackend::Data::getPhoto(const IPhotoInfo::Id& id)
    {
        //try to find cached one
        IPhotoInfo::Ptr photoInfo = m_photoInfoCache->find(id);

        if (photoInfo.get() == nullptr)  // cache miss - construct new
        {
            //basic data
            photoInfo = std::make_shared<PhotoInfo>(getPathFor(id));
            photoInfo->initID(id);

            //load tags
            Tag::TagsList tagData = getTagsFor(id);

            photoInfo->setTags(tagData);

            //load thumbnail
            const ol::Optional<QPixmap> thumbnail = getThumbnailFor(id);
            if (thumbnail)
                photoInfo->initThumbnail(*thumbnail);

            //load sha256
            const ol::Optional<IPhotoInfo::Sha256sum> sha256 = getSha256For(id);
            if (sha256)
                photoInfo->initSha256(*sha256);

            //load flags
            updateFlagsOn(photoInfo.get(), id);

            postPhotoInfoCreation(photoInfo);
        }

        return photoInfo;
    }


    Tag::TagsList ASqlBackend::Data::getTagsFor(const IPhotoInfo::Id& photoId)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        QString queryStr = QString("SELECT "
                                   "%1.id, %2.name, %1.value, %2.type "
                                   "FROM "
                                   "%1 "
                                   "JOIN "
                                   "%2 "
                                   "ON %2.id = %1.name_id "
                                   "WHERE %1.photo_id = '%3';")
                           .arg(TAB_TAGS)
                           .arg(TAB_TAG_NAMES)
                           .arg(photoId.value());

        const bool status = exec(queryStr, &query);
        Tag::TagsList tagData;
        VariantConverter convert;

        while(status && query.next())
        {
            const QString name  = query.value(1).toString();
            const QString value = query.value(2).toString();
            const unsigned int tagType = query.value(3).toInt();

            const TagNameInfo tagName(name, tagType);
            tagData[tagName] = TagValue( convert(tagName.getType(), value) );
        }

        return tagData;
    }


    ol::Optional<QPixmap> ASqlBackend::Data::getThumbnailFor(const IPhotoInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        ol::Optional<QPixmap> pixmap;
        QSqlQuery query(db);

        QString queryStr = QString("SELECT data FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_THUMBS);
        queryStr = queryStr.arg(id.value());

        const bool status = exec(queryStr, &query);

        if(status && query.next())
        {
            const QVariant variant = query.value(0);
            QByteArray data(variant.toByteArray());
            pixmap = fromPrintable(data);
        }

        return pixmap;
    }
    
    ol::Optional<IPhotoInfo::Sha256sum> ASqlBackend::Data::getSha256For(const IPhotoInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT sha256 FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_SHA256SUMS);
        queryStr = queryStr.arg(id.value());

        const bool status = exec(queryStr, &query);

        ol::Optional<IPhotoInfo::Sha256sum> result;
        if(status && query.next())
        {
            const QVariant variant = query.value(0);
            
            result = variant.toString().toStdString();
        }

        return result;
    }


    void ASqlBackend::Data::updateFlagsOn(IPhotoInfo* photoInfo, const IPhotoInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT staging_area, tags_loaded, sha256_loaded, thumbnail_loaded FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_FLAGS);
        queryStr = queryStr.arg(id.value());

        const bool status = exec(queryStr, &query);

        if (status && query.next())
        {
            QVariant variant = query.value(0);
            photoInfo->markFlag(IPhotoInfo::FlagsE::StagingArea, variant.toInt());

            variant = query.value(1);
            photoInfo->markFlag(IPhotoInfo::FlagsE::ExifLoaded, variant.toInt());

            variant = query.value(2);
            photoInfo->markFlag(IPhotoInfo::FlagsE::Sha256Loaded, variant.toInt());

            variant = query.value(3);
            photoInfo->markFlag(IPhotoInfo::FlagsE::ThumbnailLoaded, variant.toInt());
        }
    }


    QString ASqlBackend::Data::getPathFor(const IPhotoInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        QString queryStr = QString("SELECT path FROM %1 WHERE %1.id = '%2'");

        queryStr = queryStr.arg(TAB_PHOTOS);
        queryStr = queryStr.arg(id.value());

        const bool status = exec(queryStr, &query);

        QString result;
        if(status && query.next())
        {
            const QVariant path = query.value(0);

            result = path.toString();
        }

        return result;
    }


    IPhotoInfo::List ASqlBackend::Data::fetch(QSqlQuery& query)
    {
        IPhotoInfo::List collection;

        while (query.next())
        {
            const IPhotoInfo::Id id(query.value("photos_id").toInt());
            IPhotoInfo::Ptr photo = getPhoto(id);

            collection.push_back(photo);
        }

        return collection;
    }


    ///////////////////////////////////////////////////////////////////////


    ASqlBackend::ASqlBackend(): m_data(new Data(this))
    {

    }


    ASqlBackend::~ASqlBackend()
    {
        assert(m_data->m_dbOpen == false);
    }


    void ASqlBackend::setPhotoInfoCache(IPhotoInfoCache* cache)
    {
        m_data->m_photoInfoCache = cache;
    }


    void ASqlBackend::closeConnections()
    {
        {
            QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

            assert(m_data->m_dbOpen == db.isOpen());
            if (db.isValid() && db.isOpen())
            {
                m_data->m_logger->log(ILogger::Severity::Info, "ASqlBackend: closing database connections.");
                db.close();
                m_data->m_dbOpen = false;
            }
        }
        
        QSqlDatabase::removeDatabase(m_data->m_connectionName);
    }


    const QString& ASqlBackend::getConnectionName() const
    {
        return m_data->m_connectionName;
    }


    BackendStatus ASqlBackend::assureTableExists(const TableDefinition& definition) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

        QSqlQuery query(db);
        const QString showQuery = getQueryConstructor()->prepareFindTableQuery(definition.name);

        BackendStatus status = m_data->exec(showQuery, &query);

        //create table 'name' if doesn't exist
        bool empty = query.next() == false;

        if (status && empty)
        {
            QString columnsDesc;
            const int size = definition.columns.size();

            for(int i = 0; i < size; i++)
            {
                const bool notlast = i + 1 < size;
                columnsDesc += getQueryConstructor()->prepareColumnDescription(definition.columns[i]) + (notlast? ", ": "");
            }

            status = m_data->exec( getQueryConstructor()->prepareCreationQuery(definition.name, columnsDesc), &query );

            if (status && definition.keys.empty() == false)
            {
                const int keys = definition.keys.size();

                for(int i = 0; status && i < keys; i++)
                {
                    QString indexDesc;

                    indexDesc += "CREATE " + definition.keys[i].type;
                    indexDesc += " " + definition.keys[i].name + "_idx";
                    indexDesc += " ON " + definition.name;
                    indexDesc += " " + definition.keys[i].def + ";";

                    status = m_data->exec(indexDesc, &query);
                }
            }
        }

        return status;
    }


    bool ASqlBackend::exec(const QString& query, QSqlQuery* status) const
    {
        return m_data->exec(query, status);
    }


    bool ASqlBackend::transactionsReady()
    {
        return m_data->m_connectionName != "";
    }


    bool ASqlBackend::beginTransaction()
    {
        return m_data->m_transaction.begin();
    }


    bool ASqlBackend::endTransaction()
    {
        return m_data->m_transaction.end();
    }


    BackendStatus ASqlBackend::init(const ProjectInfo& prjInfo)
    {
        //store thread id for further validation
        m_data->m_database_thread_id = std::this_thread::get_id();
        m_data->m_connectionName = prjInfo.projectDir;
        m_data->m_transaction.setDBName(prjInfo.projectDir);

        BackendStatus status = prepareDB(prjInfo);
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

        m_data->m_dbHasSizeFeature = db.driver()->hasFeature(QSqlDriver::QuerySize);

        if (status)
        {
            m_data->m_dbOpen = db.open();
            status = m_data->m_dbOpen? StatusCodes::Ok: StatusCodes::OpenFailed;
        }

        if (status)
            status = checkStructure();
        else
            m_data->m_logger->log(ILogger::Severity::Error, std::string("Error opening database: ") + db.lastError().text().toStdString());

        return status;
    }


    IPhotoInfo::Ptr ASqlBackend::addPath(const QString& path)
    {
        auto photoInfo = std::make_shared<PhotoInfo>(path);

        m_data->store(photoInfo);
        m_data->postPhotoInfoCreation(photoInfo);

        return photoInfo;
    }


    bool ASqlBackend::update(const IPhotoInfo::Ptr& entry)
    {
        assert(entry->getID().valid());

        bool status = false;

        if (m_data)
            status = m_data->store(entry);
        else
            m_data->m_logger->log(ILogger::Severity::Error, "Database object does not exist.");

        return status;
    }


    bool ASqlBackend::update(const TagNameInfo& tagInfo)
    {
        assert(tagInfo.getType() != TagNameInfo::Invalid);

        bool status = false;

        if (m_data)
            status = m_data->store(tagInfo);
        else
            m_data->m_logger->log(ILogger::Severity::Error, "Database object does not exist.");

        return status;
    }


    std::deque<TagNameInfo> ASqlBackend::listTags()
    {
        std::deque<TagNameInfo> result;

        if (m_data)
            result = m_data->listTags();
        else
            m_data->m_logger->log(ILogger::Severity::Error, "Database object does not exist.");

        return result;
    }


    std::deque<QVariant> ASqlBackend::listTagValues(const TagNameInfo& tagName)
    {
        std::deque<QVariant> result;

        if (m_data)
            result = m_data->listTagValues(tagName);
        else
            m_data->m_logger->log(ILogger::Severity::Error, "Database object does not exist.");

        return result;
    }


    std::deque<QVariant> ASqlBackend::listTagValues(const TagNameInfo& tagName, const std::deque<IFilter::Ptr>& filter)
    {
        const std::deque<QVariant> result = m_data->listTagValues(tagName, filter);

        return result;
    }


    IPhotoInfo::List ASqlBackend::getAllPhotos()
    {
        std::deque<IFilter::Ptr> emptyFilter;
        return m_data->getPhotos(emptyFilter);  //like getPhotos but without any filters
    }


    IPhotoInfo::Ptr ASqlBackend::getPhoto(const IPhotoInfo::Id& id)
    {
        auto result = m_data->getPhoto(id);

        return result;
    }


    IPhotoInfo::List ASqlBackend::getPhotos(const std::deque<IFilter::Ptr>& filter)
    {
        return m_data->getPhotos(filter);
    }


    int ASqlBackend::getPhotosCount(const std::deque<IFilter::Ptr>& filter)
    {
        return m_data->getPhotosCount(filter);
    }


    void ASqlBackend::set(ILoggerFactory* logger_factory)
    {
        m_data->m_logger = logger_factory->get({"Database" ,"ASqlBackend"});
        m_data->m_transaction.set(m_data->m_logger.get());
    }


    void ASqlBackend::addEventsObserver(IBackend::IEvents* observer)
    {
        m_data->m_observers.insert(observer);
    }


    BackendStatus ASqlBackend::checkStructure()
    {
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);
        BackendStatus status = m_data->m_transaction.begin();

        //check tables existance
        if (status)
            for (TableDefinition& table: tables)
            {
                status = assureTableExists(table);

                if (!status)
                    break;
            }

        QSqlQuery query(db);

        // table 'version' cannot be empty
        if (status)
            status = m_data->exec("SELECT COUNT(*) FROM " TAB_VER ";", &query);

        if (status)
            status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;

        const QVariant rows = status? query.value(0): QVariant(0);

        //insert first entry
        if (status)
        {
            if (rows == 0)
                status = m_data->exec(QString("INSERT INTO " TAB_VER "(version) VALUES(%1);")
                                      .arg(db_version), &query);
            else
                status = checkDBVersion();
        }

        status &= m_data->m_transaction.end();

        return status;
    }

}


Database::BackendStatus Database::ASqlBackend::checkDBVersion()
{
    QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);
    QSqlQuery query(db);
    
    BackendStatus status = m_data->exec("SELECT version FROM " TAB_VER ";", &query);
    
    if (status)
        status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;
    
    if (status)
    {
        const int v = query.value(0).toInt();
        
        // More than we expect? Quit with error
        if (v > 1)
            status = StatusCodes::BadVersion;
    }
    
    return status;
}
