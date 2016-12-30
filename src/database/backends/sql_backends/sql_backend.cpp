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

#include <QBuffer>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QPixmap>

#include <OpenLibrary/utils/optional.hpp>

#include <core/tag.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <core/map_iterator.hpp>
#include <database/action.hpp>
#include <database/filter.hpp>
#include <database/iphoto_info_cache.hpp>
#include <database/project_info.hpp>
#include <database/photo_info.hpp>

#include "isql_query_constructor.hpp"
#include "tables.hpp"
#include "query_structs.hpp"
#include "sql_action_query_generator.hpp"
#include "sql_filter_query_generator.hpp"
#include "sql_query_executor.hpp"
#include "database_migrator.hpp"


// usefull links
// about insert + update/ignore: http://stackoverflow.com/questions/15277373/sqlite-upsert-update-or-insert


namespace Database
{
    namespace
    {

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


        std::deque<TagValue> flatten(const TagValue& tagValue)
        {
            const TagValue::Type type = tagValue.type();

            std::deque<TagValue> result;

            switch (type)
            {
                case TagValue::Type::Empty:
                    assert(!"empty tag value!");
                    break;

                case TagValue::Type::Date:
                case TagValue::Type::String:
                case TagValue::Type::Time:
                    result.push_back(tagValue);
                    break;

                case TagValue::Type::List:
                {
                    auto list = tagValue.getList();

                    for (const TagValue& subitem: list)
                    {
                        const std::deque<TagValue> local_result = flatten(subitem);

                        result.insert(result.end(), local_result.begin(), local_result.end());
                    }

                    break;
                }
            }

            return result;
        }


        std::deque<std::pair<TagNameInfo, TagValue>> flatten(const Tag::TagsList& tagsList)
        {
            std::deque<std::pair<TagNameInfo, TagValue>> result;

            for(const auto& tag: tagsList)
            {
                const TagValue& tagValue = tag.second;
                const std::deque<TagValue> flatList = flatten(tagValue);

                for(const TagValue& flat: flatList)
                {
                    const auto p = std::make_pair(tag.first, flat);
                    result.push_back(p);
                }
            }

            return result;
        }

    }


    /*****************************************************************************/


    struct ASqlBackend::Data
    {
            ASqlBackend* m_backend;
            QString m_connectionName;
            std::unique_ptr<ILogger> m_logger;
            SqlQueryExecutor m_executor;
            bool m_dbHasSizeFeature;
            bool m_dbOpen;

            Data(ASqlBackend* backend);
            ~Data();
            Data(const Data &) = delete;
            Data& operator=(const Data &) = delete;

            bool store(const TagValue& value, int photo_id, int name_id, int tag_id = -1) const;

            bool insert(Photo::Data &) const;
            bool insert(std::deque<Photo::Data> &) const;
            Group::Id addGroup(const Photo::Id& id) const;
            bool update(const Photo::Data &) const;

            std::deque<TagValue>  listTagValues(const TagNameInfo& tagName) const;
            std::deque<TagValue>  listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &) const;

            void                  perform(const std::deque<IFilter::Ptr> &, const std::deque<IAction::Ptr> &) const;

            std::deque<Photo::Id> getPhotos(const std::deque<IFilter::Ptr> &) const;
            std::deque<Photo::Id> dropPhotos(const std::deque<IFilter::Ptr> &) const;
            Photo::Data           getPhoto(const Photo::Id &) const;
            int                   getPhotosCount(const std::deque<IFilter::Ptr> &) const;

        private:
            bool storeData(const Photo::Data &) const;
            bool storeGeometryFor(const Photo::Id &, const QSize &) const;
            bool storeSha256(int photo_id, const Photo::Sha256sum &) const;
            bool storeTags(int photo_id, const Tag::TagsList &) const;
            bool storeFlags(const Photo::Data &) const;
            bool storeGroup(const Photo::Data &) const;

            Tag::TagsList        getTagsFor(const Photo::Id &) const;
            QSize                getGeometryFor(const Photo::Id &) const;
            ol::Optional<Photo::Sha256sum> getSha256For(const Photo::Id &) const;
            Group::Id            getGroupFor(const Photo::Id &) const;
            void    updateFlagsOn(Photo::Data &, const Photo::Id &) const;
            QString getPathFor(const Photo::Id &) const;
            std::deque<Photo::Id> fetch(QSqlQuery &) const;
            bool doesPhotoExist(const Photo::Id &) const;

            bool updateOrInsert(const UpdateQueryData &) const;
    };


    ASqlBackend::Data::Data(ASqlBackend* backend): m_backend(backend),
                                                   m_connectionName(""),
                                                   m_logger(nullptr),
                                                   m_executor(),
                                                   m_dbHasSizeFeature(false),
                                                   m_dbOpen(false)
    {

    }


    ASqlBackend::Data::~Data()
    {

    }


    bool ASqlBackend::Data::store(const TagValue& tagValue, int photo_id, int name_id, int tag_id) const
    {
        //store tag values
        bool status = true;
        const TagValue::Type type = tagValue.type();

        switch (type)
        {
            case TagValue::Type::Empty:
                assert(!"Empty tag value!");
                break;

            case TagValue::Type::List:
                assert(!"TagValue should be flat");
                break;

            case TagValue::Type::Date:
            case TagValue::Type::String:
            case TagValue::Type::Time:
            {
                QSqlDatabase db = QSqlDatabase::database(m_connectionName);
                QSqlQuery query(db);

                const QString value = tagValue.rawValue();

                InsertQueryData queryData(TAB_TAGS);
                queryData.setColumns("value", "photo_id", "name");
                queryData.setValues(value, photo_id, name_id);

                if (tag_id == -1)
                    query = m_backend->getGenericQueryGenerator()->insert(db, queryData);
                else
                {
                    UpdateQueryData updateQueryData(queryData);
                    updateQueryData.setCondition("id", QString::number(tag_id));
                    query = m_backend->getGenericQueryGenerator()->update(db, updateQueryData);
                }

                status = m_executor.exec(query);

                break;
            }
        }

        return status;
    }


    std::deque<TagValue> ASqlBackend::Data::listTagValues(const TagNameInfo& tagName) const
    {
        const int tagId = tagName.getTag();

        std::deque<TagValue> result;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        const QString query_str = QString("SELECT value FROM " TAB_TAGS " WHERE name=\"%1\";").arg(tagId);

        const bool status = m_executor.exec(query_str, &query);

        while (status && query.next())
        {
            const QString raw_value = query.value(0).toString();
            const TagValue value = TagValue::fromRaw(raw_value, tagName.getType());

            result.push_back(value);
        }

        return result;
    }


    std::deque<TagValue> ASqlBackend::Data::listTagValues(const TagNameInfo& tagName, const std::deque<IFilter::Ptr>& filter) const
    {
        const QString filterQuery = SqlFilterQueryGenerator().generate(filter);

        // from filtered photos, get info about tags used there
        // NOTE: filterQuery must go as a last item as it may contain '%X' which would ruin queryStr
        QString queryStr = "SELECT DISTINCT %2.value FROM (%3) AS distinct_select JOIN (%2) ON (photos_id=%2.photo_id) WHERE name='%1'";

        queryStr = queryStr.arg(tagName.getTag());
        queryStr = queryStr.arg(TAB_TAGS);
        queryStr = queryStr.arg(filterQuery);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        std::deque<TagValue> result;
        const bool status = m_executor.exec(queryStr, &query);

        if (status)
        {
            while (status && query.next())
            {
                const QString raw_value = query.value(0).toString();
                const TagValue value = TagValue::fromRaw(raw_value, tagName.getType());

                result.push_back(value);
            }
        }

        return result;
    }


    void ASqlBackend::Data::perform(const std::deque<IFilter::Ptr>& filter, const std::deque<IAction::Ptr>& actions) const
    {
        for(auto action: actions)
        {
            const QString queryStr = SqlFilterQueryGenerator().generate(filter);
            const QString actionStr = SqlActionQueryGenerator().generate(action);
            const QString finalQuery = actionStr.arg(queryStr);

            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            QSqlQuery query(db);

            m_executor.exec(finalQuery, &query);
        }
    }


    std::deque<Photo::Id> ASqlBackend::Data::getPhotos(const std::deque<IFilter::Ptr>& filter) const
    {
        const QString queryStr = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor.exec(queryStr, &query);
        auto result = fetch(query);

        return result;
    }


    int ASqlBackend::Data::getPhotosCount(const std::deque<IFilter::Ptr>& filter) const
    {
        const QString queryStr = SqlFilterQueryGenerator().generate(filter);

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


    std::deque<Photo::Id>  ASqlBackend::Data::dropPhotos(const std::deque<IFilter::Ptr>& filter) const
    {
        const QString filterQuery = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        //collect ids of photos to be dropped
        std::deque<Photo::Id> ids;
        bool status = m_executor.exec(filterQuery, &query);

        if (status)
        {
            while(query.next())
            {
                Photo::Id id( query.value(0).toUInt() );

                ids.push_back(id);
            }
        }

        //from filtered photos, get info about tags used there
        std::vector<QString> queries =
        {
            QString("CREATE TEMPORARY TABLE drop_indices AS %1").arg(filterQuery),
            QString("DELETE FROM " TAB_FLAGS       " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_SHA256SUMS  " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_TAGS        " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_THUMBS      " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_PHOTOS      " WHERE id IN (SELECT * FROM drop_indices)"),
            QString("DROP TABLE drop_indices")
        };

        status = db.transaction();

        if (status)
            status = m_executor.exec(queries, &query);

        if (status)
            status = db.commit();
        else
            db.rollback();

        return ids;
    }


    bool ASqlBackend::Data::storeData(const Photo::Data& data) const
    {
        assert(data.id);

        //store used tags
        Tag::TagsList tags = data.tags;

        bool status = storeTags(data.id, tags);

        if (status && data.getFlag(Photo::FlagsE::GeometryLoaded) > 0)
            status = storeGeometryFor(data.id, data.geometry);

        if (status && data.getFlag(Photo::FlagsE::Sha256Loaded) > 0)
            status = storeSha256(data.id, data.sha256Sum);

        if (status)
            status = storeFlags(data);

        if (status)
            status = storeGroup(data);

        return status;
    }


    bool ASqlBackend::Data::storeGeometryFor(const Photo::Id& photo_id, const QSize& geometry) const
    {
        UpdateQueryData data(TAB_GEOMETRY);
        data.setCondition("photo_id", QString::number(photo_id));
        data.setColumns("photo_id", "width", "height");
        data.setValues(QString::number(photo_id), QString::number(geometry.width()), QString::number(geometry.height()) );

        const bool status = updateOrInsert(data);

        return status;
    }


    bool ASqlBackend::Data::storeSha256(int photo_id, const Photo::Sha256sum& sha256) const
    {
        UpdateQueryData data(TAB_SHA256SUMS);
        data.setCondition("photo_id", QString::number(photo_id));
        data.setColumns("photo_id", "sha256");
        data.setValues(QString::number(photo_id), sha256.constData());

        const bool status = updateOrInsert(data);

        return status;
    }


    bool ASqlBackend::Data::storeTags(int photo_id, const Tag::TagsList& tagsList) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        bool status = true;

        // gather ids for current set of tag for photo_id
        const QString tagIdsQuery = QString("SELECT id FROM %1 WHERE photo_id=\"%2\"")
                                    .arg(TAB_TAGS)
                                    .arg(photo_id);

        status = m_executor.exec(tagIdsQuery, &query);

        // store tags
        if (status)
        {
            // read tag ids from query
            std::deque<int> currentIds;

            while (query.next())
            {
                const QVariant idRaw = query.value(0);
                const int id = idRaw.toInt();

                currentIds.push_back(id);
            }

            // convert map with possible lists into flat list of pairs
            const std::deque<std::pair<TagNameInfo, TagValue>> tagsFlatList = flatten(tagsList);

            // difference between current set in db and new set of tags
            const int currentIdsSize = static_cast<int>( currentIds.size() );
            const int tagsListSize   = static_cast<int>( tagsFlatList.size() );
            const int diff = currentIdsSize - tagsListSize;

            // more tags in db?, delete surplus
            if (diff > 0)
            {
                QStringList idsToDelete;
                for(auto it = currentIds.end() - diff; it != currentIds.end(); ++it)
                    idsToDelete.append( QString::number(*it) );

                const QString idsToDeleteStr = idsToDelete.join(", ");

                const QString deleteQuery = QString("DELETE FROM %1 WHERE id IN (%2)")
                                                .arg(TAB_TAGS)
                                                .arg(idsToDeleteStr);

                status = m_executor.exec(deleteQuery, &query);
            }

            // override existing tags and then insert (if nothing left to override)
            std::size_t counter = 0;

            for (auto it = tagsFlatList.begin(); status && it != tagsFlatList.end(); ++it, counter++)
            {
                const TagValue& value = it->second;
                const int name = it->first.getTag();
                const int tag_id = counter < currentIds.size()? currentIds[counter]: -1;  // try to override ids of tags already stored

                status = store(value, photo_id, name, tag_id);
            }

        }

        return status;
    }


    bool ASqlBackend::Data::storeFlags(const Photo::Data& photoData) const
    {
        UpdateQueryData queryInfo(TAB_FLAGS);
        queryInfo.setCondition("photo_id", QString::number(photoData.id));
        queryInfo.setColumns("photo_id", "staging_area", "tags_loaded", "sha256_loaded", "thumbnail_loaded", FLAG_GEOM_LOADED);
        queryInfo.setValues(QString::number(photoData.id),
                             photoData.getFlag(Photo::FlagsE::StagingArea),
                             photoData.getFlag(Photo::FlagsE::ExifLoaded),
                             photoData.getFlag(Photo::FlagsE::Sha256Loaded),
                             photoData.getFlag(Photo::FlagsE::ThumbnailLoaded),
                             photoData.getFlag(Photo::FlagsE::GeometryLoaded));

        const bool status = updateOrInsert(queryInfo);

        return status;
    }


    bool ASqlBackend::Data::storeGroup(const Photo::Data& data) const
    {
        bool status = true;

        if (data.group_id == 0)
        {
            // TODO: remove if exists
        }
        else
        {
            UpdateQueryData queryInfo(TAB_GROUPS_MEMBERS);
            queryInfo.setCondition("photo_id", QString::number(data.id));
            queryInfo.setColumns("group_id", "photo_id");
            queryInfo.setValues(QString::number(data.group_id),
                                QString::number(data.id)
            );

            status = updateOrInsert(queryInfo);
        }

        return status;
    }


    bool ASqlBackend::Data::insert(Photo::Data& data) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        bool status = true;

        //store path and date
        Photo::Id id = data.id;
        assert(id.valid() == false);

        InsertQueryData insertData(TAB_PHOTOS);

        insertData.setColumns("path", "store_date");
        insertData.setValues(data.path, InsertQueryData::Value::CurrentTime);
        insertData.setColumns("id");
        insertData.setValues(InsertQueryData::Value::Null);

        QSqlQuery query = m_backend->getGenericQueryGenerator()->insert(db, insertData);

        if (status)
            status = m_executor.exec(query);

        //update id
        if (status)                                    //Get Id from database after insert
        {
            QVariant photo_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            status = photo_id.isValid();

            if (status)
                id = Photo::Id(photo_id.toInt());
        }

        //make sure id is set
        if (status)
            status = id.valid();

        if (status)
        {
            assert(data.id.valid() == false || data.id == id);
            data.id = id;
        }

        if (status)
            status = storeData(data);

        return status;
    }


    bool ASqlBackend::Data::insert(std::deque<Photo::Data>& data_set) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        Transaction transaction(db);
        bool status = transaction.begin();

        for(Photo::Data& data: data_set)
        {
            if (status)
                status = insert(data);

            if (status == false)
                break;
        }

        if (status)
            status = transaction.commit();

        return status;
    }


    Group::Id ASqlBackend::Data::addGroup(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        Group::Id grp_id = 0;

        InsertQueryData insertData(TAB_GROUPS);

        insertData.setColumns("id", "representative_id");
        insertData.setValues(InsertQueryData::Value::Null, id);

        QSqlQuery query = m_backend->getGenericQueryGenerator()->insert(db, insertData);

        bool status = m_executor.exec(query);

        //update id
        if (status)                                    //Get Id from database after insert
        {
            QVariant photo_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            status = photo_id.isValid();

            if (status)
                grp_id = Photo::Id(photo_id.toInt());
        }

        return grp_id;
    }


    bool ASqlBackend::Data::update(const Photo::Data& data) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        Transaction transaction(db);
        bool status = transaction.begin();

        if (status)
            status = storeData(data);

        if (status)
            status = transaction.commit();

        return status;
    }


    Photo::Data ASqlBackend::Data::getPhoto(const Photo::Id& id) const
    {
        const bool valid_id = doesPhotoExist(id);
        assert(valid_id);

        Photo::Data photoData;

        if (valid_id)
        {
            photoData.path = getPathFor(id);
            photoData.id   = id;
            photoData.tags = getTagsFor(id);

            //load geometry
            const QSize geometry = getGeometryFor(id);
            if (geometry.isValid())
                photoData.geometry = geometry;

            //load sha256
            const ol::Optional<Photo::Sha256sum> sha256 = getSha256For(id);
            if (sha256)
                photoData.sha256Sum = *sha256;

            //load flags
            updateFlagsOn(photoData, id);

            // load group
            photoData.group_id = getGroupFor(id);
        }

        return photoData;
    }


    Tag::TagsList ASqlBackend::Data::getTagsFor(const Photo::Id& photoId) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        const QString queryStr = QString("SELECT "
                                         "%1.id, %1.name, %1.value "
                                         "FROM "
                                         "%1 "
                                         "WHERE %1.photo_id = '%2'")
                                 .arg(TAB_TAGS)
                                 .arg(photoId.value());

        const bool status = m_executor.exec(queryStr, &query);
        Tag::TagsList tagData;

        while(status && query.next())
        {
            const BaseTagsList tagNameType = static_cast<BaseTagsList>( query.value(1).toInt() );
            const QVariant value = query.value(2);
            const TagNameInfo tagName(tagNameType);
            const TagNameInfo::Type tagType = tagName.getType();

            TagValue tagValue;

            switch(tagType)
            {
                case TagNameInfo::Type::Date:
                    tagValue = TagValue(value.toDate());
                    break;

                case TagNameInfo::Type::Time:
                    tagValue = TagValue(value.toTime());
                    break;

                case TagNameInfo::Type::String:
                    tagValue = TagValue(value.toString());
                    break;

                case TagNameInfo::Type::Invalid:
                    assert(!"Invalid case");
                    break;
            }

            const bool multivalue = tagName.isMultiValue();

            if (multivalue)   // accumulate vs override
            {
                // insert() will add empty List if there is no entry for given key.
                // otherwise will do nothing.
                auto insert_it = tagData.insert( std::make_pair(tagName, TagValueTraits<TagValue::Type::List>::StorageType()) );
                auto it = insert_it.first;   // get regular map iterator
                TagValue& listTagValue = it->second;
                TagValueTraits<TagValue::Type::List>::StorageType& values = listTagValue.getList();

                values.push_back(tagValue);
            }
            else
                tagData[tagName] = tagValue;

        }

        return tagData;
    }


    QSize ASqlBackend::Data::getGeometryFor(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        QSize geoemtry;
        QSqlQuery query(db);

        const QString queryStr = QString("SELECT width,height FROM %1 WHERE %1.photo_id = '%2'")
                                 .arg(TAB_GEOMETRY)
                                 .arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        if (status && query.next())
        {
            const QVariant widthRaw = query.value(0);
            const QVariant heightRaw = query.value(1);

            const int width = widthRaw.toInt();
            const int height = heightRaw.toInt();

            geoemtry = QSize(width, height);
        }

        return geoemtry;
    }


    ol::Optional<Photo::Sha256sum> ASqlBackend::Data::getSha256For(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT sha256 FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_SHA256SUMS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        ol::Optional<Photo::Sha256sum> result;
        if(status && query.next())
        {
            const QVariant variant = query.value(0);

            result->append(variant.toString());
        }

        return result;
    }


    Group::Id ASqlBackend::Data::getGroupFor(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT group_id FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_GROUPS_MEMBERS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        Group::Id result = 0;
        if (status && query.next())
        {
            const QVariant variant = query.value(0);

            result = variant.toInt();
        }

        return result;
    }


    void ASqlBackend::Data::updateFlagsOn(Photo::Data& photoData, const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT staging_area, tags_loaded, sha256_loaded, thumbnail_loaded, geometry_loaded FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_FLAGS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        if (status && query.next())
        {
            QVariant variant = query.value(0);
            photoData.flags[Photo::FlagsE::StagingArea] = variant.toInt();

            variant = query.value(1);
            photoData.flags[Photo::FlagsE::ExifLoaded] = variant.toInt();

            variant = query.value(2);
            photoData.flags[Photo::FlagsE::Sha256Loaded] = variant.toInt();

            variant = query.value(3);
            photoData.flags[Photo::FlagsE::ThumbnailLoaded] = variant.toInt();

            variant = query.value(4);
            photoData.flags[Photo::FlagsE::GeometryLoaded] = variant.toInt();
        }
    }


    QString ASqlBackend::Data::getPathFor(const Photo::Id& id) const
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


    std::deque<Photo::Id> ASqlBackend::Data::fetch(QSqlQuery& query) const
    {
        std::deque<Photo::Id> collection;

        while (query.next())
        {
            const Photo::Id id(query.value("photos_id").toInt());

            collection.push_back(id);
        }

        return collection;
    }


    bool ASqlBackend::Data::doesPhotoExist(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        QString queryStr = QString("SELECT id FROM %1 WHERE %1.id = '%2'");

        queryStr = queryStr.arg(TAB_PHOTOS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        Photo::Id result;
        if(status && query.next())
        {
            const QVariant p_id = query.value(0);

            static_assert(sizeof(decltype(p_id.toInt())) == sizeof(Photo::Id::type), "Incompatible types for id");
            result = Photo::Id(p_id.toInt());
        }

        return result == id;
    }


    bool ASqlBackend::Data::updateOrInsert(const UpdateQueryData& queryInfo) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        QSqlQuery query = m_backend->getGenericQueryGenerator()->update(db, queryInfo);

        bool status = m_executor.exec(query);

        if (status)
        {
            const int affected_rows = query.numRowsAffected();

            if (affected_rows == 0)
            {
                query = m_backend->getGenericQueryGenerator()->insert(db, queryInfo);
                status = m_executor.exec(query);
            }
        }

        return status;
    }


    ///////////////////////////////////////////////////////////////////////


    ASqlBackend::ASqlBackend(): m_data(new Data(this))
    {

    }


    ASqlBackend::~ASqlBackend()
    {
        assert(m_data->m_dbOpen == false);
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


    BackendStatus ASqlBackend::ensureTableExists(const TableDefinition& definition) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);

        QSqlQuery query(db);
        const QString showQuery = getGenericQueryGenerator()->prepareFindTableQuery(definition.name);

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
                    getGenericQueryGenerator()->getTypeFor(definition.columns[i].purpose),
                    definition.columns[i].type_definition
                };

                const QString type = types.join(" ").simplified();

                const bool notlast = i + 1 < size;
                columnsDesc += definition.columns[i].name + " ";
                columnsDesc += type;
                columnsDesc += notlast? ", ": "";
            }

            status = m_data->m_executor.exec( getGenericQueryGenerator()->prepareCreationQuery(definition.name, columnsDesc), &query );

            if (status && definition.keys.empty() == false)
            {
                const int keys = definition.keys.size();

                for(int i = 0; status && i < keys; i++)
                    createKey(definition.keys[i], definition.name, query);
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
        m_data->m_connectionName = prjInfo.databaseLocation;

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
            ErrorStream(m_data->m_logger.get()) << "Error opening database: " << db.lastError().text();

        return status;
    }


    bool ASqlBackend::addPhotos(std::deque<Photo::Data>& data)
    {
        const bool status = m_data->insert(data);

        return status;
    }


    Group::Id ASqlBackend::addGroup(const Photo::Id& id)
    {
        const Group::Id group = m_data->addGroup(id);

        return group;
    }


    bool ASqlBackend::update(const Photo::Data& data)
    {
        assert(data.id.valid());

        bool status = false;

        if (m_data)
            status = m_data->update(data);
        else
            m_data->m_logger->error("Database object does not exist.");

        return status;
    }


    std::deque<TagNameInfo> ASqlBackend::listTags()
    {
        assert(!"Not implemented");
        return std::deque<TagNameInfo>();
    }


    std::deque<TagValue> ASqlBackend::listTagValues(const TagNameInfo& tagName)
    {
        std::deque<TagValue> result;

        if (m_data)
            result = m_data->listTagValues(tagName);
        else
            m_data->m_logger->error("Database object does not exist.");

        return result;
    }


    std::deque<TagValue> ASqlBackend::listTagValues(const TagNameInfo& tagName, const std::deque<IFilter::Ptr>& filter)
    {
        const std::deque<TagValue> result = m_data->listTagValues(tagName, filter);

        return result;
    }


    std::deque<Photo::Id> ASqlBackend::getAllPhotos()
    {
        std::deque<IFilter::Ptr> emptyFilter;
        return m_data->getPhotos(emptyFilter);  //like getPhotos but without any filters
    }


    Photo::Data ASqlBackend::getPhoto(const Photo::Id& id)
    {
        auto result = m_data->getPhoto(id);

        return result;
    }


    std::deque<Photo::Id> ASqlBackend::getPhotos(const std::deque<IFilter::Ptr>& filter)
    {
        return m_data->getPhotos(filter);
    }


    int ASqlBackend::getPhotosCount(const std::deque<IFilter::Ptr>& filter)
    {
        return m_data->getPhotosCount(filter);
    }


    void ASqlBackend::perform(const std::deque<IFilter::Ptr>& filter, const std::deque<IAction::Ptr>& action)
    {
        return m_data->perform(filter, action);
    }


    std::deque<Photo::Id> ASqlBackend::dropPhotos(const std::deque<IFilter::Ptr>& filter)
    {
        return m_data->dropPhotos(filter);
    }


    void ASqlBackend::set(ILoggerFactory* logger_factory)
    {
        m_data->m_logger = logger_factory->get({"Database" ,"ASqlBackend"});
        m_data->m_executor.set(m_data->m_logger.get());
    }


    BackendStatus ASqlBackend::checkStructure()
    {
        QSqlDatabase db = QSqlDatabase::database(m_data->m_connectionName);
        Transaction transaction(db);

        BackendStatus status = transaction.begin();

        //check tables existance
        if (status)
            for (const auto& table: tables)
            {
                status = ensureTableExists(table.second);

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

        switch (v)
        {
            case 1:   // append column 'role' to FLAGS table
                if (status)
                    status = m_data->m_executor.exec("ALTER TABLE " TAB_FLAGS " ADD " FLAG_ROLE " INT NOT NULL DEFAULT 0", &query);

            case 2:   // current version, break updgrades chain
                break;

            default:
                // Unknown version? Quit with error
                status = StatusCodes::BadVersion;
                break;
        }

        // store new version in db
        if (status && v < db_version)
        {
            const QString queryStr = QString("UPDATE " TAB_VER " SET version = %1 WHERE version = %2").arg(db_version).arg(v);
            status = m_data->m_executor.exec(queryStr, &query);
        }
    }

    return status;
}


bool Database::ASqlBackend::createKey(const Database::TableDefinition::KeyDefinition& key, const QString& tableName, QSqlQuery& query) const
{
    QString indexDesc;

    indexDesc += "CREATE " + key.type;
    indexDesc += " " + key.name + "_idx";
    indexDesc += " ON " + tableName;
    indexDesc += " " + key.def + ";";

    const bool status = m_data->m_executor.exec(indexDesc, &query);

    return status;
}
