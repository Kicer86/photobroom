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
#include <deque>

#include "database/ibackend.hpp"
#include "sql_backend_base_export.h"

class QSqlQuery;
class QSqlDatabase;

struct IPhotoInfoManager;

namespace Database
{

    class Entry;
    class InsertQueryData;
    struct ISqlQueryConstructor;
    struct TableDefinition;

    class SQL_BACKEND_BASE_EXPORT ASqlBackend: public Database::IBackend
    {
        public:
            struct Data;

            ASqlBackend();
            ASqlBackend(const ASqlBackend& other) = delete;
            virtual ~ASqlBackend();

            ASqlBackend& operator=(const ASqlBackend& other) = delete;
            bool operator==(const ASqlBackend& other) = delete;

            void setPhotoInfoCreator(IPhotoInfoCreator *) override;
            void setPhotoInfoManager(IPhotoInfoCache *) override;
            void closeConnections();

        protected:
            //will be called from init(). Prepare QSqlDatabase object here
            virtual bool prepareDB(QSqlDatabase *, const QString& location) = 0;

            //Creates sql database. Can be called in onAfterOpen in backends which need it
            virtual bool createDB(const QString &);

            //called after db open. May be used by backends for some extra steps after open.
            virtual bool onAfterOpen();

            //make sure table exists. Makes sure a table maching TableDefinition exists in database
            virtual bool assureTableExists(const TableDefinition &) const;

            //execute query. Function for inheriting classes
            virtual bool exec(const QString &, QSqlQuery *) const;

            virtual const ISqlQueryConstructor* getQueryConstructor() const = 0;

            virtual void set(ILogger *) override;
            
        private:
            std::unique_ptr<Data> m_data;

            virtual bool transactionsReady() override;
            virtual bool beginTransaction() override;
            virtual bool endTransaction() override;

            virtual bool init(const QString &) override final;
            virtual IPhotoInfo::Ptr addPath(const QString &) override final;
            virtual bool update(const IPhotoInfo::Ptr &) override final;

            virtual std::deque<TagNameInfo> listTags() override final;
            virtual std::set<TagValueInfo> listTagValues(const TagNameInfo&) override final;
            virtual std::deque<TagValueInfo> listTagValues(const TagNameInfo &, const std::deque<IFilter::Ptr> &) override final;
            virtual IPhotoInfo::List getAllPhotos() override final;
            virtual IPhotoInfo::Ptr getPhoto(const IPhotoInfo::Id &) override final;
            virtual IPhotoInfo::List getPhotos(const std::deque<IFilter::Ptr> &) override final;

            bool checkStructure();
    };

}

#endif // ASQLBACKEND_H

