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
#include <database/iphoto_info_cache.hpp>
#include <database/project_info.hpp>
#include <database/backends/photo_info.hpp>

#include "table_definition.hpp"
#include "sql_query_constructor.hpp"
#include "tables.hpp"
#include "query_structs.hpp"
#include "sql_select_query_generator.hpp"
#include "variant_converter.hpp"
#include "sql_query_executor.hpp"
#include "database_migrator.hpp"


namespace Database
{
    namespace
    {
        QByteArray toPrintable(const QImage& image)
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);

            image.save(&buffer, "JPEG");

            return bytes.toBase64();
        }

        QImage fromPrintable(const QByteArray& data)
        {
            const QByteArray bytes = QByteArray::fromBase64(data);
            QImage image;

            const bool status = image.loadFromData(bytes, "JPEG");

            if (status == false)
            {
                //TODO: load thumbnail from file
            }

            return image;
        }


        struct Transaction
        {
            Transaction(QSqlDatabase& db): m_db(db)
            {

            }

            ~Transaction()
            {

            }

            Transaction(const Transaction &) = delete;
            Transaction& operator=(const Transaction &) = delete;

            BackendStatus begin()
            {
                const BackendStatus status = m_db.transaction()? StatusCodes::Ok: StatusCodes::TransactionFailed;

                return status;
            }

            BackendStatus commit()
            {
                const BackendStatus status = m_db.commit()? StatusCodes::Ok: StatusCodes::TransactionCommitFailed;

                return status;
            }

        private:
            QSqlDatabase& m_db;

        };

    }

    struct StorePhoto;

    /*****************************************************************************/


    struct ASqlBackend::Data
    {
            ASqlBackend* m_backend;
            QString m_connectionName;
            IPhotoInfoCache* m_photoInfoCache;
            std::unique_ptr<ILogger> m_logger;
            std::set<IBackend::IEvents *> m_observers;
            SqlQueryExecutor m_executor;
            bool m_dbHasSizeFeature;
            bool m_dbOpen;

            Data(ASqlBackend* backend);
            ~Data();
            Data(const Data &) = delete;
            Data& operator=(const Data &) = delete;

            ol::Optional<unsigned int> store(const TagNameInfo& nameInfo) const;
            bool store(const TagValue& value, int photo_id, int tag_id) const;
            bool store(const IPhotoInfo::Ptr& data);
            IPhotoInfo::Ptr getPhoto(const IPhotoInfo::Id &);
            std::deque<TagNameInfo> listTags() const;
            std::deque<QVariant> listTagValues(const TagNameInfo& tagName);
            std::deque<QVariant> listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &);
            IPhotoInfo::List getPhotos(const std::deque<IFilter::Ptr> &);
            int getPhotosCount(const std::deque<IFilter::Ptr> &);
            std::deque<IPhotoInfo::Ptr> dropPhotos(const std::deque<IFilter::Ptr> &);

            void postPhotoInfoCreation(const IPhotoInfo::Ptr &) const;

        private:
            ol::Optional<unsigned int> findTagByName(const QString& name) const;
            QString generateFilterQuery(const std::deque<IFilter::Ptr>& filter);

            bool storeThumbnail(int photo_id, const QImage &) const;
            bool storeSha256(int photo_id, const IPhotoInfo::Sha256sum &) const;
            bool storeTags(int photo_id, const Tag::TagsList &) const;
            bool storeFlags(int photo_id, const IPhotoInfo::Ptr &) const;

            Tag::TagsList getTagsFor(const IPhotoInfo::Id &);
            ol::Optional<QImage> getThumbnailFor(const IPhotoInfo::Id &);
            ol::Optional<IPhotoInfo::Sha256sum> getSha256For(const IPhotoInfo::Id &);
            void updateFlagsOn(IPhotoInfo*, const IPhotoInfo::Id &);
            QString getPathFor(const IPhotoInfo::Id &);
            IPhotoInfo::List fetch(QSqlQuery &);
    };


    ASqlBackend::Data::Data(ASqlBackend* backend): m_backend(backend),
                                                   m_connectionName(""),
                                                   m_photoInfoCache(nullptr),
                                                   m_logger(nullptr),
                                                   m_observers(),
                                                   m_executor(),
                                                   m_dbHasSizeFeature(false),
                                                   m_dbOpen(false)
    {

    }


    ASqlBackend::Data::~Data()
    {

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

            const bool status = m_executor.exec(queryStr, &query);

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

        const bool status = m_executor.exec(query_str, &query);

        return status;
    }


    std::deque<TagNameInfo> ASqlBackend::Data::listTags() const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        const QString query_str("SELECT name, type FROM " TAB_TAG_NAMES ";");

        const bool status = m_executor.exec(query_str, &query);
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

            const bool status = m_executor.exec(query_str, &query);

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
        const bool status = m_executor.exec(queryStr, &query);

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

        m_executor.exec(queryStr, &query);
        auto result = fetch(query);

        return result;
    }


    int ASqlBackend::Data::getPhotosCount(const std::deque<IFilter::Ptr>& filter)
    {
        const QString queryStr = generateFilterQuery(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor.exec(queryStr, &query);

        int result = 0;

        if (m_dbHasSizeFeature)
            result = query.size();
        else
            result = query.next()? 1: 0;

        return result;
    }


    std::deque<IPhotoInfo::Ptr> ASqlBackend::Data::dropPhotos(const std::deque<IFilter::Ptr>& filters)
    {
        const QString filterQuery = generateFilterQuery(filters);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        //collect ids of photos to be dropped
        std::deque<IPhotoInfo::Id> ids;
        bool status = m_executor.exec(filterQuery, &query);

        if (status)
        {
            while(query.next())
            {
                IPhotoInfo::Id id( query.value(0).toUInt() );

                ids.push_back(id);
            }
        }

        //from filtered photos, get info about tags used there
        SqlMultiQuery queries =
        {
            QString("DELETE FROM " TAB_PHOTOS      " WHERE id IN (%1)").arg(filterQuery),
            QString("DELETE FROM " TAB_FLAGS       " WHERE photo_id IN (%1)").arg(filterQuery),
            QString("DELETE FROM " TAB_SHA256SUMS  " WHERE photo_id IN (%1)").arg(filterQuery),
            QString("DELETE FROM " TAB_TAGS        " WHERE photo_id IN (%1)").arg(filterQuery),
            QString("DELETE FROM " TAB_THUMBS      " WHERE photo_id IN (%1)").arg(filterQuery)
        };

        status = db.transaction();

        if (status)
            status = m_executor.exec(queries, &query);

        if (status)
            status = db.commit();
        else
            db.rollback();

        //convert ids to photo ptrs and remove them from cache
        std::deque<IPhotoInfo::Ptr> photos;
        for(const IPhotoInfo::Id& id: ids)
        {
            IPhotoInfo::Ptr photo = m_photoInfoCache->find(id);
            assert(photo.get() != nullptr);

            photos.push_back(photo);

            m_photoInfoCache->forget(id);
        }

        return photos;
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
        const bool status = m_executor.exec(find_tag_query, &query);

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


    bool ASqlBackend::Data::storeThumbnail(int photo_id, const QImage& pixmap) const
    {
        InsertQueryData data(TAB_THUMBS);

        data.setColumns("photo_id", "data");
        data.setValues(QString::number(photo_id), QString(toPrintable(pixmap)) );

        SqlMultiQuery queryStrs = m_backend->getQueryConstructor()->insertOrUpdate(data);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = m_executor.exec(queryStrs, &query);

        return status;
    }


    bool ASqlBackend::Data::storeSha256(int photo_id, const IPhotoInfo::Sha256sum& sha256) const
    {
        InsertQueryData data(TAB_SHA256SUMS);

        data.setColumns("photo_id", "sha256");
        data.setValues(QString::number(photo_id), sha256.c_str());

        SqlMultiQuery queryStrs = m_backend->getQueryConstructor()->insertOrUpdate(data);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = m_executor.exec(queryStrs, &query);

        return status;
    }


    bool ASqlBackend::Data::storeTags(int photo_id, const Tag::TagsList& tagsList) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = true;

        //remove all tags already attached to photo. TODO: maybe some inteligence here?
        const QString deleteQuery = QString("DELETE FROM " TAB_TAGS " WHERE photo_id=\"%1\"").arg(photo_id);
        status = m_executor.exec(deleteQuery, &query);

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
        bool status = m_executor.exec(queryStrs, &query);

        return status;
    }


    bool ASqlBackend::Data::store(const IPhotoInfo::Ptr& data)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        Transaction transaction(db);
        bool status = transaction.begin();

        //store path and sha256
        IPhotoInfo::Id id = data->getID();
        const bool updating = id.valid();
        const bool inserting = !updating;

        InsertQueryData insertData(TAB_PHOTOS);
        insertData.setColumns("path", "store_date");
        insertData.setValues(data->getPath(), InsertQueryData::Value::CurrentTime);

        SqlMultiQuery queryStrs;

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
            status = m_executor.exec(queryStrs, &query);

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
            status = transaction.commit();

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
            const ol::Optional<QImage> thumbnail = getThumbnailFor(id);
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

        const bool status = m_executor.exec(queryStr, &query);
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


    ol::Optional<QImage> ASqlBackend::Data::getThumbnailFor(const IPhotoInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        ol::Optional<QImage> image;
        QSqlQuery query(db);

        QString queryStr = QString("SELECT data FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_THUMBS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        if(status && query.next())
        {
            const QVariant variant = query.value(0);
            QByteArray data(variant.toByteArray());
            image = fromPrintable(data);
        }

        return image;
    }

    ol::Optional<IPhotoInfo::Sha256sum> ASqlBackend::Data::getSha256For(const IPhotoInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT sha256 FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_SHA256SUMS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

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

        const bool status = m_executor.exec(queryStr, &query);

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

        const bool status = m_executor.exec(queryStr, &query);

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


    bool ASqlBackend::dbOpened()
    {
        return true;
    }


    BackendStatus ASqlBackend::assureTableExists(const TableDefinition& definition) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

        QSqlQuery query(db);
        const QString showQuery = getQueryConstructor()->prepareFindTableQuery(definition.name);

        BackendStatus status = m_data->m_executor.exec(showQuery, &query);

        //create table 'name' if doesn't exist
        bool empty = query.next() == false;

        if (status && empty)
        {
            QString columnsDesc;
            const int size = definition.columns.size();

            for(int i = 0; i < size; i++)
            {
                const QStringList types =
                {
                    getQueryConstructor()->getTypeFor(definition.columns[i].purpose),
                    definition.columns[i].type_definition
                };

                const QString type = types.join(" ").simplified();

                const bool notlast = i + 1 < size;
                columnsDesc += definition.columns[i].name + " ";
                columnsDesc += type;
                columnsDesc += notlast? ", ": "";
            }

            status = m_data->m_executor.exec( getQueryConstructor()->prepareCreationQuery(definition.name, columnsDesc), &query );

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

                    status = m_data->m_executor.exec(indexDesc, &query);
                }
            }
        }

        return status;
    }


    bool ASqlBackend::exec(const QString& query, QSqlQuery* status) const
    {
        return m_data->m_executor.exec(query, status);
    }


    BackendStatus ASqlBackend::init(const ProjectInfo& prjInfo)
    {
        //store thread id for further validation
        m_data->m_executor.set( std::this_thread::get_id() );
        m_data->m_connectionName = prjInfo.projectDir;

        BackendStatus status = prepareDB(prjInfo);
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

        m_data->m_dbHasSizeFeature = db.driver()->hasFeature(QSqlDriver::QuerySize);

        if (status)
        {
            m_data->m_dbOpen = db.open();
            status = m_data->m_dbOpen? StatusCodes::Ok: StatusCodes::OpenFailed;
        }

        if (status)
            status = dbOpened()? StatusCodes::Ok: StatusCodes::OpenFailed;

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


    std::deque<IPhotoInfo::Ptr> ASqlBackend::dropPhotos(const std::deque<IFilter::Ptr>& filter)
    {
        return m_data->dropPhotos(filter);
    }


    void ASqlBackend::set(ILoggerFactory* logger_factory)
    {
        m_data->m_logger = logger_factory->get({"Database" ,"ASqlBackend"});
        m_data->m_executor.set(m_data->m_logger.get());
    }


    void ASqlBackend::addEventsObserver(IBackend::IEvents* observer)
    {
        m_data->m_observers.insert(observer);
    }


    BackendStatus ASqlBackend::checkStructure()
    {
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);
        Transaction transaction(db);

        BackendStatus status = transaction.begin();

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
            status = m_data->m_executor.exec("SELECT COUNT(*) FROM " TAB_VER ";", &query);

        if (status)
            status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;

        const QVariant rows = status? query.value(0): QVariant(0);

        //insert first entry
        if (status)
        {
            if (rows == 0)
                status = m_data->m_executor.exec(QString("INSERT INTO " TAB_VER "(version) VALUES(%1);")
                                      .arg(db_version), &query);
            else
                status = checkDBVersion();
        }

        status &= transaction.commit();

        return status;
    }

}


Database::BackendStatus Database::ASqlBackend::checkDBVersion()
{
    QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

    QSqlQuery query(db);

    BackendStatus status = m_data->m_executor.exec("SELECT version FROM " TAB_VER ";", &query);

    if (status)
        status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;

    if (status)
    {
        const int v = query.value(0).toInt();

        // More than we expect? Quit with error
        if (v > 1)
            status = StatusCodes::BadVersion;
    }

    /*
    if (status)
    {
        DatabaseMigrator migrator(db, &m_data->m_executor);

        if (migrator.needsMigration())
            status = migrator.migrate()? StatusCodes::Ok: StatusCodes::MigrationFailed;
    }
    */

    return status;
}
