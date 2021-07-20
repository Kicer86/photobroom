/*
 * Photo Broom - photos management tool.
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

#ifndef ASQLBACKEND_HPP
#define ASQLBACKEND_HPP

#include <memory>
#include <vector>
#include <vector>

#include "core/lazy_ptr.hpp"
#include "database/ibackend.hpp"
#include "group_operator.hpp"
#include "people_information_accessor.hpp"
#include "photo_change_log_operator.hpp"
#include "photo_operator.hpp"
#include "sql_backend_base_export.h"
#include "sql_query_executor.hpp"
#include "table_definition.hpp"
#include "transaction.hpp"

class QSqlQuery;
class QSqlDatabase;

struct IPhotoInfoManager;

namespace Database
{
    class Entry;
    class InsertQueryData;
    class UpdateQueryData;
    struct IGenericSqlQueryGenerator;
    struct TableDefinition;


    class SQL_BACKEND_BASE_EXPORT ASqlBackend: public Database::IBackend
    {
        public:
            ASqlBackend(ILogger *);
            ASqlBackend(const ASqlBackend& other) = delete;
            virtual ~ASqlBackend();

            ASqlBackend& operator=(const ASqlBackend& other) = delete;
            bool operator==(const ASqlBackend& other) = delete;

            void closeConnections() override;

            /**
             * \brief Get connection name
             * \return connection name
             *
             * Method gives access to connection name.
             * It can be used to establish connection
             * with database with QSqlDatabase or to create
             * new connection. \see prepareDB()
             */
            const QString& getConnectionName() const;

            GroupOperator& groupOperator() override;
            PhotoOperator& photoOperator() override;
            PhotoChangeLogOperator& photoChangeLogOperator() override;
            IPeopleInformationAccessor& peopleInformationAccessor() override;

        protected:
            /**
             * \brief database preparation
             * \param location where database should be created.
             *
             * Method called from init().
             * Its purpose is to create new database connection named by
             * getConnectionName() with QSqlDatabase::addDatabase().             *
             */
            virtual BackendStatus prepareDB(const ProjectInfo& location) = 0;

            /**
             * \brief called when DB was opened.
             * \return operation status
             *
             * Called by init()
             * Backend may perform some extra setup.
             * If false is returned init() exists with StatusCodes::OpenFailed.
             */
            virtual bool dbOpened();


            /**
            * \brief Make sure given table exists in database
            * \return true on success
            *
            * If table does not exists, will be created.
            */
            BackendStatus ensureTableExists(const TableDefinition &) const;

            /**
             * \brief Execute query
             * \param query query to be executed
             * \param query_obj instance of QSqlQuery object
             * \return true if succeed
             */
            virtual bool exec(const QString& query, QSqlQuery* query_obj) const;

            /**
             * \brief IGenericSqlQueryGenerator accessor
             * \return instance of IGenericSqlQueryGenerator
             */
            virtual const IGenericSqlQueryGenerator* getGenericQueryGenerator() const = 0;

        private:
            std::unique_ptr<GroupOperator> m_groupOperator;
            std::unique_ptr<PhotoOperator> m_photoOperator;
            std::unique_ptr<PhotoChangeLogOperator> m_photoChangeLogOperator;
            lazy_ptr<IPeopleInformationAccessor, std::function<IPeopleInformationAccessor*()>> m_peopleInfoAccessor;
            mutable NestedTransaction m_tr_db;
            QString m_connectionName;
            std::unique_ptr<ILogger> m_logger;
            SqlQueryExecutor m_executor;
            bool m_dbHasSizeFeature;
            bool m_dbOpen;

            // Database::IBackend:
            BackendStatus init(const ProjectInfo &) override final;
            bool addPhotos(std::vector<Photo::DataDelta> &) override final;
            bool update(const std::vector<Photo::DataDelta> &) override final;

            std::vector<TagValue>    listTagValues(const TagTypes &, const Filter &) override final;

            Photo::Data              getPhoto(const Photo::Id &) override final;
            Photo::DataDelta         getPhotoDelta(const Photo::Id &, const std::set<Photo::Field> & = {}) override final;
            int                      getPhotosCount(const Filter &) override final;
            void                     set(const Photo::Id &, const QString &, int) override final;
            std::optional<int>       get(const Photo::Id &, const QString &) override final;

            void setThumbnail(const Photo::Id &, const QByteArray &) override;
            QByteArray getThumbnail(const Photo::Id &) override;

            std::vector<Photo::Id> markStagedAsReviewed() override final;
            //

            // general helpers
            BackendStatus checkStructure();
            Database::BackendStatus checkDBVersion();
            bool updateOrInsert(const UpdateQueryData &) const;

            // helpers for sql operations
            std::vector<PersonInfo> listPeople(const std::vector<Photo::Id> &);

            bool createKey(const Database::TableDefinition::KeyDefinition &, const QString &, QSqlQuery &) const;

            bool store(const TagValue& value, int photo_id, int name_id, int tag_id = -1) const;

            bool insert(std::vector<Photo::DataDelta> &);

            void introduce(Photo::DataDelta &);
            bool storeData(const Photo::DataDelta &);
            bool storeGeometryFor(const Photo::Id &, const QSize &) const;
            bool storeSha256(int photo_id, const Photo::Sha256sum &) const;
            bool storeTags(int photo_id, const Tag::TagsList &) const;
            bool storeFlags(const Photo::Id &, const Photo::FlagValues &) const;
            bool storeGroup(const Photo::Id &, const GroupInfo &) const;

            Tag::TagsList        getTagsFor(const Photo::Id &) const;
            QSize                getGeometryFor(const Photo::Id &) const;
            std::optional<Photo::Sha256sum> getSha256For(const Photo::Id &) const;
            GroupInfo            getGroupFor(const Photo::Id &) const;
            Photo::FlagValues    getFlagsFor(const Photo::Id &) const;
            QString getPathFor(const Photo::Id &) const;
            bool doesPhotoExist(const Photo::Id &) const;
    };
}

#endif // ASQLBACKEND_HPP

