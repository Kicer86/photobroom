/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "photo_operator.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <core/ilogger.hpp>
#include <database/ibackend.hpp>

#include "isql_query_executor.hpp"
#include "sql_filter_query_generator.hpp"
#include "tables.hpp"


namespace Database
{

    static std::map<const char *, const char *> tables_to_clear =
    {
        { TAB_FLAGS,          "photo_id" },
        { TAB_GENERAL_FLAGS,  "photo_id" },
        { TAB_GEOMETRY,       "photo_id" },
        { TAB_GROUPS_MEMBERS, "photo_id" },
        { TAB_PEOPLE,         "photo_id" },
        { TAB_SHA256SUMS,     "photo_id" },
        { TAB_TAGS,           "photo_id" },
        { TAB_THUMBS,         "photo_id" },
        { TAB_PHOTOS,         "id"       }
    };


    PhotoOperator::PhotoOperator(const QString& connection, ISqlQueryExecutor* executor, ILogger* logger, IBackend* backend):
        m_connectionName(connection),
        m_executor(executor),
        m_logger(logger),
        m_backend(backend)
    {

    }


    bool PhotoOperator::removePhoto(const Photo::Id& id)
    {
        auto id_filter = std::make_shared<FilterPhotosWithId>();
        id_filter->filter = id;

        const std::vector<IFilter::Ptr> filters = {id_filter};

        return removePhotos(filters);
    }

    bool PhotoOperator::removePhotos(const std::vector<IFilter::Ptr>& filter)
    {
        const QString filterQuery = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        //collect ids of photos to be dropped
        std::vector<Photo::Id> ids;
        bool status = m_executor->exec(filterQuery, &query);

        if (status)
        {
            while(query.next())
            {
                const Photo::Id id(query.value(0).toUInt());

                ids.push_back(id);
            }
        }

        //from filtered photos, get info about tags used there
        std::vector<QString> queries =
        {
            QString("CREATE TEMPORARY TABLE drop_indices AS %1").arg(filterQuery),
            QString("DELETE FROM " TAB_FLAGS             " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_GENERAL_FLAGS     " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_GEOMETRY          " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            // There should be no data in groups and group members TODO: check + remove group if not true
            QString("DELETE FROM " TAB_PEOPLE            " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_PHOTOS_CHANGE_LOG " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_SHA256SUMS        " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_TAGS              " WHERE photo_id IN (SELECT * FROM drop_indices)"),
            QString("DELETE FROM " TAB_THUMBS            " WHERE photo_id IN (SELECT * FROM drop_indices)"),

            QString("DELETE FROM " TAB_PHOTOS            " WHERE id IN (SELECT * FROM drop_indices)"),
            QString("DROP TABLE drop_indices")
        };

        status = db.transaction();

        if (status)
            status = m_executor->exec(queries, &query);

        if (status)
            status = db.commit();
        else
            db.rollback();

        emit m_backend->photosRemoved(ids);

        return status;
    }


    std::vector<Photo::Id> PhotoOperator::onPhotos(const std::vector<IFilter::Ptr> &, const Actions& action)
    {
        return {};
    }


    std::vector<Photo::Id> PhotoOperator::getPhotos(const std::vector<IFilter::Ptr>& filter)
    {
        const QString queryStr = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor->exec(queryStr, &query);
        auto result = fetch(query);

        return result;
    }


    /**
     * \brief collect photo ids SELECTed by SQL query
     * \param query SQL SELECT query which returns photo ids
     * \return unique list of photo ids
     */
    std::vector<Photo::Id> PhotoOperator::fetch(QSqlQuery& query) const
    {
        std::vector<Photo::Id> collection;

        while (query.next())
        {
            const Photo::Id id(query.value("photos.id").toInt());

            collection.push_back(id);
        }

#ifndef NDEBUG
        // verify there are no duplicates in results
        auto copy_of_collection = collection;
        std::sort(copy_of_collection.begin(), copy_of_collection.end());

        assert(std::unique(copy_of_collection.begin(), copy_of_collection.end()) == copy_of_collection.end());
#endif

        return collection;
    }

}
