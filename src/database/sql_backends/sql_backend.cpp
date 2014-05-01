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

#include <boost/optional.hpp>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <core/tag.hpp>
#include <core/task_executor.hpp>
#include <configuration/constants.hpp>
#include <database/filter.hpp>

#include "table_definition.hpp"
#include "sqldbquery.hpp"


namespace Database
{
    namespace
    {
        //check for proper sizes
        static_assert(sizeof(int) >= 4, "int is smaller than MySQL's equivalent");

        const char db_version[] = "0.01";

        TableDefinition
            table_versionHistory(TAB_VER_HIST,
                                    {
                                        { "id", ColDefinition::Type::ID },
                                        "version DECIMAL(4,2) NOT NULL",       //xx.yy
                                        "date TIMESTAMP NOT NULL"
                                    }
                                 );

        TableDefinition
            table_photos(TAB_PHOTOS,
                            {
                                { "id", ColDefinition::Type::ID },
                                "hash VARCHAR(256) NOT NULL",
                                "path VARCHAR(1024) NOT NULL",
                                "store_date TIMESTAMP NOT NULL"
                            },
                            {
                                { "hash", "INDEX", "(hash)"  },   //256 limit required by MySQL
                                { "path", "INDEX", "(path)" }     //1024 limit required by MySQL
                            }
                         );


        TableDefinition
            table_tag_names(TAB_TAG_NAMES,
                            {
                                { "id", ColDefinition::Type::ID },
                                QString("name VARCHAR(%1) NOT NULL").arg(Consts::Constraints::database_tag_name_len),
                                        "type INT NOT NULL"
                            },
                            {
                                { "name", "UNIQUE INDEX", "(name)" }    //.arg(Consts::Constraints::database_tag_name_len)} required by MySQL
                            }
                           );

        TableDefinition
            table_tags(TAB_TAGS,
                            {
                                { "id", ColDefinition::Type::ID },
                                QString("value VARCHAR(%1)").arg(Consts::Constraints::database_tag_value_len),
                                "name_id INTEGER NOT NULL",
                                "photo_id INTEGER NOT NULL",
                                "FOREIGN KEY(photo_id) REFERENCES photos(id)",
                                "FOREIGN KEY(name_id) REFERENCES " TAB_TAG_NAMES "(id)"
                            }
                       );
    }

    struct StorePhoto;

    struct ASqlBackend::Data
    {
        QSqlDatabase m_db;
        ASqlBackend* m_backend;

        Data(ASqlBackend* backend);
        ~Data();
        Data(const Data &) = delete;
        Data& operator=(const Data &) = delete;

        bool exec(const QString& query, QSqlQuery* result) const;
        bool createDB(const QString& dbName) const;
        boost::optional<unsigned int> store(const TagNameInfo& nameInfo) const;
        bool store(const PhotoInfo::Ptr& data);
        std::vector<TagNameInfo> listTags() const;
        std::set<TagValueInfo> listTagValues(const TagNameInfo& tagName);
        QueryList getPhotos(const Filter& filter);

        private:
            friend struct StorePhoto;
            boost::optional<unsigned int> findTagByName(const QString& name) const;
            QString generateFilterQuery(const Filter& filter);
            bool applyTags(int photo_id, const std::shared_ptr<ITagData>& tags) const;

            //for friends:
            bool storePhoto(const PhotoInfo::Ptr& data);
    };


    struct StorePhoto: ITaskExecutor::ITask
    {
        ASqlBackend::Data* m_data;
        PhotoInfo::Ptr m_photo;

        StorePhoto(ASqlBackend::Data* data, const PhotoInfo::Ptr& photo): m_data(data), m_photo(photo)
        {
        }

        StorePhoto(const StorePhoto &) = delete;
        StorePhoto& operator=(const StorePhoto &) = delete;

        virtual bool canBePerformed() const
        {
            return true;
        }

        virtual std::string name() const
        {
            return std::string("Storing photo ") + m_photo->getPath();
        }

        virtual void perform()
        {
            bool status = m_data->storePhoto(m_photo); //call store from ASqlBackend::Data

            if (status == false)
                std::cerr << "error while storing photo in database" << std::endl;
        }
    };


    ASqlBackend::Data::Data(ASqlBackend* backend): m_db(), m_backend(backend) {}


    ASqlBackend::Data::~Data()
    {

    }


    bool ASqlBackend::Data::exec(const QString& query, QSqlQuery* result) const
    {
        const bool status = result->exec(query);

        if (status == false)
            std::cerr << "SQLBackend: error: " << result->lastError().text().toStdString()
                        << " while performing query: " << query.toStdString() << std::endl;

        return status;
    }


    bool ASqlBackend::Data::createDB(const QString& dbName) const
    {
        //check if database exists
        QSqlQuery query(m_db);
        bool status = exec(QString("SHOW DATABASES LIKE '%1';").arg(dbName), &query);

        //create database if doesn't exists
        bool empty = query.next() == false;
        if (status && empty)
            status = exec(QString("CREATE DATABASE `%1`;").arg(dbName), &query);

        //switch to database
        if (status)
            status = exec(QString("USE %1;").arg(dbName), &query);

        return status;
    }


    boost::optional<unsigned int> ASqlBackend::Data::store(const TagNameInfo& nameInfo) const
    {
        const QString& name = nameInfo.getName();
        const int type = nameInfo.getType();
        QSqlQuery query(m_db);

        //check if tag exists
        boost::optional<unsigned int> tagId = findTagByName(name);

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


    bool ASqlBackend::Data::store(const PhotoInfo::Ptr& data)
    {
        auto task = std::make_shared<StorePhoto>(this, data);
        TaskExecutorConstructor::get()->add(task);

        return true;
    }


    std::vector<TagNameInfo> ASqlBackend::Data::listTags() const
    {
        QSqlQuery query(m_db);
        const QString query_str("SELECT name, type FROM " TAB_TAG_NAMES ";");

        const bool status = exec(query_str, &query);
        std::vector<TagNameInfo> result;

        while (status && query.next())
        {
            const QString name       = query.value(0).toString();
            const unsigned int value = query.value(1).toUInt();

            TagNameInfo tagName(name, value);
            result.push_back(tagName);
        }

        return result;
    }


    std::set<TagValueInfo> ASqlBackend::Data::listTagValues(const TagNameInfo& tagName)
    {
        const boost::optional<unsigned int> tagId = findTagByName(tagName);

        std::set<TagValueInfo> result;

        if (tagId)
        {
            QSqlQuery query(m_db);
            const QString query_str = QString("SELECT value FROM " TAB_TAGS " WHERE name_id=\"%1\";")
                                        .arg(*tagId);

            const bool status = exec(query_str, &query);

            while (status && query.next())
            {
                const QString value = query.value(0).toString();

                TagValueInfo valueInfo(value);
                result.insert(valueInfo);
            }
        }

        return result;
    }


    QueryList ASqlBackend::Data::getPhotos(const Filter& filter)
    {
        const QString filterStr = generateFilterQuery(filter);

        QSqlQuery query(m_db);
        const QString queryStr = QString("SELECT %1.id, %1.path, %1.hash, %2.type, %2.name, %3.value"
                                            " FROM %3 LEFT JOIN (%1, %2)"
                                            " ON (%1.id=%3.photo_id AND %2.id=%3.name_id) %4 ORDER BY %1.id")
                                            .arg(TAB_PHOTOS).arg(TAB_TAG_NAMES).arg(TAB_TAGS).arg(filterStr);

        exec(queryStr, &query);
        SqlDBQuery* dbQuery = new SqlDBQuery(query, m_backend);
        InterfaceContainer<IQuery> container(dbQuery);

        QueryList result(container);

        return result;
    }

    boost::optional<unsigned int> ASqlBackend::Data::findTagByName(const QString& name) const
    {
        QSqlQuery query(m_db);
        const QString find_tag_query = QString("SELECT id FROM " TAB_TAG_NAMES " WHERE name =\"%1\";").arg(name);
        const bool status = exec(find_tag_query, &query);

        boost::optional<unsigned int> result;
        if (status && query.next())
            result = query.value(0).toInt();

        return result;
    }


    QString ASqlBackend::Data::generateFilterQuery(const Filter& filter)
    {
        QString result;
        const std::vector <Database::FilterDescription >& filters = filter.getFilters();

        if (filters.empty() == false)
            result += "WHERE";

        for(size_t i = 0; i < filters.size(); i++)
        {
            const Database::FilterDescription& f = filters[i];

            if (i > 0)
                result += " AND";

            result += QString(" name = '%1' AND value = '%2'").arg(f.tagName, f.tagValue);
        }

        return result;
    }


    bool ASqlBackend::Data::applyTags(int photo_id, const std::shared_ptr<ITagData>& tags) const
    {
        QSqlQuery query(m_db);
        bool status = true;

        ITagData::TagsList tagsList = tags->getTags();
        for (auto it = tagsList.begin(); status && it != tagsList.end(); ++it)
        {
            //store tag
            const boost::optional<unsigned int> tag_id = store(it->first);

            if (tag_id)
            {
                //store tag values
                const ITagData::ValuesSet& values = it->second;
                for (auto it_v = values.cbegin(); it_v != values.cend(); ++it_v)
                {
                    const TagValueInfo& valueInfo = *it_v;

                    const QString query_str =
                        QString("INSERT INTO " TAB_TAGS
                                "(id, value, photo_id, name_id) VALUES(NULL, \"%1\", \"%2\", \"%3\");"
                            ).arg(valueInfo.value())
                            .arg(photo_id)
                            .arg(*tag_id);

                    status = exec(query_str, &query);
                }
            }
            else
                status = false;
        }

        return status;
    }


    bool ASqlBackend::Data::storePhoto(const PhotoInfo::Ptr& data)
    {
        QSqlQuery query(m_db);

        //store path and hash
        const QString query_str =
            QString("INSERT INTO " TAB_PHOTOS
                    "(id, store_date, path, hash) VALUES(NULL, CURRENT_TIMESTAMP, \"%1\", \"%2\");"
                    ).arg(data->getPath().c_str())
                    .arg(data->getHash().c_str());

        bool status = exec(query_str, &query);
        QVariant photo_id;

        if (status)
        {
            photo_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            status = photo_id.isValid();
        }

        //store used tags
        std::shared_ptr<ITagData> tags = data->getTags();

        if (status)
            status = applyTags(photo_id.toInt(), tags);

        return status;
    }


    ///////////////////////////////////////////////////////////////////////


    ASqlBackend::ASqlBackend(): m_data(new Data(this))
    {

    }


    ASqlBackend::~ASqlBackend()
    {

    }


    void ASqlBackend::closeConnections()
    {
        if (m_data->m_db.isValid() && m_data->m_db.isOpen())
        {
            std::cout << "ASqlBackend: closing database connections." << std::endl;
            m_data->m_db.close();

            // Reset belowe is necessary.
            // There is a problem:when application is being closed, all qt resources (libraries etc) are being removed.
            // And it may happend that a driver for particular sql database will be removed before database is destroyed.
            // This will lead to crash as in database's destructor calls to driver are made.
            m_data.reset(nullptr);        //destroy database.
        }
    }


    QString ASqlBackend::prepareCreationQuery(const QString& name, const QString& columns) const
    {
        return QString("CREATE TABLE %1(%2);").arg(name).arg(columns);
    }


    QString ASqlBackend::prepareFindTableQuery(const QString& name) const
    {
        return QString("SHOW TABLES LIKE '%1';").arg(name);
    }


    bool ASqlBackend::createDB(const char* name)
    {
        return m_data->createDB(name);
    }


    bool ASqlBackend::onAfterOpen()
    {
        return true;
    }


    bool ASqlBackend::assureTableExists(const TableDefinition& definition) const
    {
        QSqlQuery query(m_data->m_db);
        const QString showQuery = prepareFindTableQuery(definition.name);

        bool status = m_data->exec(showQuery, &query);

        //create table 'name' if doesn't exist
        bool empty = query.next() == false;
        if (status && empty)
        {
            QString columnsDesc;
            const int size = definition.columns.size();
            for(int i = 0; i < size; i++)
            {
                const bool notlast = i + 1 < size;
                columnsDesc += prepareColumnDescription(definition.columns[i]) + (notlast? ", ": "");
            }

            status = m_data->exec( prepareCreationQuery(definition.name, columnsDesc), &query );

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


    bool ASqlBackend::init(const char* dbName)
    {
        bool status = prepareDB(&m_data->m_db, dbName);

        if (status)
            status = m_data->m_db.open();

        if (status)
            status = onAfterOpen();

        if (status)
            status = checkStructure();
        else
            std::cerr << "SQLBackend: error opening database: " << m_data->m_db.lastError().text().toStdString() << std::endl;

        //TODO: crash when status == false;
        return status;
    }


    bool ASqlBackend::store(const PhotoInfo::Ptr& entry)
    {
        bool status = false;

        if (m_data)
            status = m_data->store(entry);
        else
            std::cerr << "ASqlBackend: database object does not exist." << std::endl;

        return status;
    }


    std::vector<TagNameInfo> ASqlBackend::listTags()
    {
        std::vector<TagNameInfo> result;

        if (m_data)
            result = m_data->listTags();
        else
            std::cerr << "ASqlBackend: database object does not exist." << std::endl;

        return result;
    }


    std::set<TagValueInfo> ASqlBackend::listTagValues(const TagNameInfo& tagName)
    {
        std::set<TagValueInfo> result;

        if (m_data)
            result = m_data->listTagValues(tagName);
        else
            std::cerr << "ASqlBackend: database object does not exist." << std::endl;

        return result;
    }


    QueryList ASqlBackend::getAllPhotos()
    {
        return m_data->getPhotos(Filter());  //like getPhotos but without any filters
    }


    QueryList ASqlBackend::getPhotos(const Filter& filter)
    {
        return m_data->getPhotos(filter);
    }


    TagData ASqlBackend::getTagsFor(const PhotoIterator& iterator)
    {
        const IQuery* const iquery = iterator.query();
        const QVariant idRaw = iquery->getField(IQuery::Fields::Id);
        const unsigned int photoId = idRaw.toInt();

        QSqlQuery query(m_data->m_db);

        QString queryStr = QString("SELECT "
                                   "%1.id, %2.name, %1.value, %1.name_id "
                                   "FROM "
                                   "%1 "
                                   "JOIN "
                                   "%2 "
                                   "ON %2.id = %1.name_id "
                                   "WHERE %1.photo_id = '%3';")
                                   .arg(TAB_TAGS)
                                   .arg(TAB_TAG_NAMES)
                                   .arg(photoId);

        const bool status = m_data->exec(queryStr, &query);
        TagData tagData;

        while(status && query.next())
        {
            const QString name  = query.value(1).toString();
            const QString value = query.value(2).toString();
            const unsigned int tagType = query.value(3).toInt();

            tagData.setTag(TagNameInfo(name, tagType), value);
        }

        return tagData;
    }


    bool ASqlBackend::checkStructure()
    {
        bool status = true;

        //check if table 'version_history' exists
        if (status)
            status = assureTableExists(table_versionHistory);

        QSqlQuery query(m_data->m_db);

        //at least one row must be present in table 'version_history'
        if (status)
            status = m_data->exec("SELECT COUNT(*) FROM " TAB_VER_HIST ";", &query);

        if (status)
            status = query.next() == true;

        const QVariant rows = status? query.value(0): QVariant(0);

        //insert first entry
        if (status && rows == 0)
            status = m_data->exec(QString("INSERT INTO " TAB_VER_HIST "(version, date)"
                                          " VALUES(%1, CURRENT_TIMESTAMP);")
                                         .arg(db_version), &query);

        //TODO: check last entry with current version

        //photos table
        if (status)
            status = assureTableExists(table_photos);

        //tag types
        if (status)
            status = assureTableExists(table_tag_names);

        //tags table
        if (status)
            status = assureTableExists(table_tags);

        /*
        if (status)
            status = addDefaultTagsDefinitions();
        */

        return status;
    }

 }
