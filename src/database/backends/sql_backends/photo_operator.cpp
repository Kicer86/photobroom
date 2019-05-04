/*
 * Class for performing operations on photos
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


    bool PhotoOperator::removePhotos(const std::vector<Photo::Id>& ids)
    {
        bool status = true;

        Photo::Id representativePhoto;
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        try
        {
            DB_ERR_ON_FALSE(db.transaction());
            QSqlQuery query(db);

            for(const Photo::Id& id: ids)
                for(const auto& d: tables_to_clear)
                {
                    const char* name = d.first;
                    const char* col  = d.second;

                    const QString query_str = QString("DELETE FROM %1 WHERE %2=%3")
                                                .arg(name)
                                                .arg(col)
                                                .arg(id);

                    DB_ERR_ON_FALSE(m_executor->exec(query_str, &query));
                }

            DB_ERR_ON_FALSE(db.commit());

            emit m_backend->photosRemoved(ids);
        }
        catch(const db_error& ex)
        {
            db.rollback();
            status = false;

            m_logger->error(ex.what());
        }

        return status;
    }
}
