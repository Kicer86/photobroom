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

#ifndef ASQLBACKEND_H
#define ASQLBACKEND_H

#include <memory>
#include <vector>
#include <vector>

#include "database/ibackend.hpp"
#include "group_operator.hpp"
#include "sql_backend_base_export.h"
#include "table_definition.hpp"

class QSqlQuery;
class QSqlDatabase;

struct IPhotoInfoManager;

namespace Database
{

    class Entry;
    class InsertQueryData;
    struct IGenericSqlQueryGenerator;
    struct IGroupOperator;
    struct TableDefinition;

    class SQL_BACKEND_BASE_EXPORT ASqlBackend: public Database::IBackend
    {
        public:
            struct Data;

            ASqlBackend(ILoggerFactory *);
            ASqlBackend(const ASqlBackend& other) = delete;
            virtual ~ASqlBackend();

            ASqlBackend& operator=(const ASqlBackend& other) = delete;
            bool operator==(const ASqlBackend& other) = delete;

            void closeConnections() override;

            const QString& getConnectionName() const;

            virtual IGroupOperator* groupOperator() override;

        protected:
            //will be called from init(). Prepare QSqlDatabase object here
            virtual BackendStatus prepareDB(const ProjectInfo& location) = 0;

            // called when db opened. Backend may perform some extra setup
            virtual bool dbOpened();

            //make sure table exists. Makes sure a table maching TableDefinition exists in database
            BackendStatus ensureTableExists(const TableDefinition &) const;

            //execute query. Function for inheriting classes
            virtual bool exec(const QString &, QSqlQuery *) const;

            virtual const IGenericSqlQueryGenerator* getGenericQueryGenerator() const = 0;

        private:
            std::unique_ptr<Data> m_data;
            std::unique_ptr<GroupOperator> m_groupOperator;

            // Database::IBackend:
            BackendStatus init(const ProjectInfo &) override final;
            bool addPhotos(std::vector<Photo::DataDelta> &) override final;
            bool update(const Photo::DataDelta &) override final;

            std::vector<TagNameInfo> listTags() override final;
            std::vector<TagValue>    listTagValues(const TagNameInfo &, const std::vector<IFilter::Ptr> &) override final;

            std::vector<Photo::Id>   getAllPhotos() override final;
            std::vector<Photo::Id>   getPhotos(const std::vector<IFilter::Ptr> &) override final;
            std::vector<Photo::Id>   dropPhotos(const std::vector<IFilter::Ptr> &) override final;
            Photo::Data              getPhoto(const Photo::Id &) override final;
            int                      getPhotosCount(const std::vector<IFilter::Ptr> &) override final;
            std::vector<PersonName>  listPeople() override final;
            std::vector<PersonInfo>  listPeople(const Photo::Id &) override final;
            PersonName               person(const Person::Id &) override final;
            Person::Id               store(const PersonName &) override final;
            PersonInfo::Id           store(const PersonInfo &) override final;
            void                     set(const Photo::Id &, const QString &, int) override final;
            std::optional<int>       get(const Photo::Id &, const QString &) override final;

            void perform(const std::vector<IFilter::Ptr> &, const std::vector<IAction::Ptr> &) override final;
            std::vector<Photo::Id> markStagedAsReviewed() override final;
            //

            PersonName    person(const QString &) const;
            BackendStatus checkStructure();
            Database::BackendStatus checkDBVersion();

            std::vector<PersonInfo> listPeople(const std::vector<Photo::Id> &);
            PersonInfo::Id storePerson(const PersonInfo &);
            void dropPersonInfo(const PersonInfo::Id &);

            bool createKey(const Database::TableDefinition::KeyDefinition &, const QString &, QSqlQuery &) const;
    };

}

#endif // ASQLBACKEND_H

