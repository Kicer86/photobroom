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

#include "sql_backend.hpp"

#include <chrono>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <thread>

#include <QDate>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QVariant>
#include <QPixmap>

#include <core/base_tags.hpp>
#include <core/tag.hpp>
#include <core/task_executor.hpp>
#include <core/ilogger.hpp>
#include <core/ilogger_factory.hpp>
#include <core/map_iterator.hpp>
#include <database/filter.hpp>
#include <database/project_info.hpp>

#include "isql_query_constructor.hpp"
#include "tables.hpp"
#include "query_structs.hpp"
#include "sql_filter_query_generator.hpp"


// useful links
// about insert + update/ignore: http://stackoverflow.com/questions/15277373/sqlite-upsert-update-or-insert


namespace Database
{

    ASqlBackend::ASqlBackend(ILogger* l):
        m_connectionName(""),
        m_logger(nullptr),
        m_executor(),
        m_dbHasSizeFeature(false),
        m_dbOpen(false)
    {
        m_logger = l->subLogger({"ASqlBackend"});
        m_executor.set(m_logger.get());
    }


    ASqlBackend::~ASqlBackend()
    {
        assert(m_dbOpen == false);
    }


    /**
     * \brief close database connection
     */
    void ASqlBackend::closeConnections()
    {
        // use scope here so all Qt objects are destroyed before removeDatabase call
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);

            if (db.isValid() && db.isOpen())
            {
                m_logger->log(ILogger::Severity::Info, "ASqlBackend: closing database connections.");
                db.close();
                m_dbOpen = false;
            }
        }

        QSqlDatabase::removeDatabase(m_connectionName);
    }


    const QString& ASqlBackend::getConnectionName() const
    {
        return m_connectionName;
    }


    GroupOperator* ASqlBackend::groupOperator()
    {
        // this lazy initialization is kind of a workaround:
        // getGenericQueryGenerator() may not work properly in
        // ASqlBackend's constructor as it is virtual
        if (m_groupOperator.get() == nullptr)
            m_groupOperator = std::make_unique<GroupOperator>(m_connectionName,
                                                              getGenericQueryGenerator(),
                                                              &m_executor,
                                                              m_logger.get(),
                                                              this
                                                             );

        return m_groupOperator.get();
    }


    PhotoOperator* ASqlBackend::photoOperator()
    {
        if (m_photoOperator.get() == nullptr)
            m_photoOperator = std::make_unique<PhotoOperator>(m_connectionName,
                                                              &m_executor,
                                                              m_logger.get(),
                                                              this
                                                             );

        return m_photoOperator.get();
    }


    PhotoChangeLogOperator* ASqlBackend::photoChangeLogOperator()
    {
        if (m_photoChangeLogOperator.get() == nullptr)
            m_photoChangeLogOperator =
                std::make_unique<PhotoChangeLogOperator>(m_connectionName,
                                                         getGenericQueryGenerator(),
                                                         &m_executor,
                                                         m_logger.get(),
                                                         this
                                                        );

        return m_photoChangeLogOperator.get();
    }


    bool ASqlBackend::dbOpened()
    {
        return true;
    }


    BackendStatus ASqlBackend::ensureTableExists(const TableDefinition& definition) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        QSqlQuery query(db);
        const QString showQuery = getGenericQueryGenerator()->prepareFindTableQuery(definition.name);

        BackendStatus status = m_executor.exec(showQuery, &query);

        //create table 'name' if doesn't exist
        bool empty = query.next() == false;

        if (status && empty)
        {
            QString columnsDesc;
            const std::size_t size = definition.columns.size();

            for(std::size_t i = 0; i < size; i++)
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

            status = m_executor.exec( getGenericQueryGenerator()->prepareCreationQuery(definition.name, columnsDesc), &query );

            if (status && definition.keys.empty() == false)
            {
                const std::size_t keys = definition.keys.size();

                for(std::size_t i = 0; status && i < keys; i++)
                    createKey(definition.keys[i], definition.name, query);
            }
        }

        return status;
    }


    bool ASqlBackend::exec(const QString& query, QSqlQuery* status) const
    {
        return m_executor.exec(query, status);
    }


    /**
     * \brief initialize database connection
     * \param prjInfo database details
     * \return operation status
     */
    BackendStatus ASqlBackend::init(const ProjectInfo& prjInfo)
    {
        //store thread id for further validation
        m_executor.set( std::this_thread::get_id() );
        m_connectionName = prjInfo.databaseLocation;
        m_tr_db.setConnectionName(m_connectionName);

        BackendStatus status = StatusCodes::Ok;
        QSqlDatabase db;

        try
        {
            DB_ERROR_ON_FALSE(prepareDB(prjInfo), StatusCodes::OpenFailed);

            db = QSqlDatabase::database(m_connectionName);

            m_dbHasSizeFeature = db.driver()->hasFeature(QSqlDriver::QuerySize);
            m_dbOpen = db.open();

            DB_ERROR_ON_FALSE(m_dbOpen, StatusCodes::OpenFailed);
            DB_ERROR_ON_FALSE(dbOpened(), StatusCodes::OpenFailed);
            DB_ERROR_ON_FALSE(checkStructure(), StatusCodes::GeneralError);
        }
        catch(const db_error& err)
        {
            m_logger->error(QString("Error opening database: %1").arg(db.lastError().text()));
            status = err.status();
        }

        return status;
    }


    bool ASqlBackend::addPhotos(std::vector<Photo::DataDelta>& data)
    {
        const bool status = insert(data);

        std::vector<Photo::Id> photos;
        photos.reserve(data.size());

        for(std::size_t i = 0; i < data.size(); i++)
            photos.push_back(data[i].getId());

        emit photosAdded(photos);

        return status;
    }


    bool ASqlBackend::update(const Photo::DataDelta& data)
    {
        assert(data.getId().valid());

        bool status = true;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        Transaction transaction(m_tr_db);

        try
        {
            DB_ERR_ON_FALSE(transaction.begin());
            DB_ERR_ON_FALSE(storeData(data));
            DB_ERR_ON_FALSE(transaction.commit());

            emit photoModified(data.getId());
        }
        catch(const db_error& error)
        {
            m_logger->error(error.what());
            status = false;
        }

        return status;
    }


    std::vector<TagTypeInfo> ASqlBackend::listTags()
    {
        assert(!"Not implemented");
        return std::vector<TagTypeInfo>();
    }


    std::vector<TagValue> ASqlBackend::listTagValues(const TagTypeInfo& tagName, const std::vector<IFilter::Ptr>& filter)
    {
        std::vector<TagValue> result;

        const QString filterQuery = SqlFilterQueryGenerator().generate(filter);

        // from filtered photos, get info about tags used there
        // NOTE: filterQuery must go as a last item as it may contain '%X' which would ruin queryStr
        // TODO: consider DISTINCT removal, just do some post process
        QString queryStr = "SELECT DISTINCT %2.value FROM (%2) JOIN (%3) ON (%3.id = %2.photo_id) WHERE name='%1' AND photos.id IN(%4)";

        queryStr = queryStr.arg(tagName.getTag());
        queryStr = queryStr.arg(TAB_TAGS);
        queryStr = queryStr.arg(TAB_PHOTOS);
        queryStr = queryStr.arg(filterQuery);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        const bool status = m_executor.exec(queryStr, &query);

        if (status)
        {
            while (status && query.next())
            {
                const QString raw_value = query.value(0).toString();
                const TagValue value = TagValue::fromRaw(raw_value, BaseTags::getType(tagName.getTag()));

                // we do not expect empty values (see store() for tags)
                assert(raw_value.isEmpty() == false);

                if (raw_value.isEmpty() == false)
                    result.push_back(value);
            }
        }

        return result;
    }


    std::vector<Photo::Id> ASqlBackend::getAllPhotos()
    {
        std::vector<IFilter::Ptr> emptyFilter;
        return getPhotos(emptyFilter);  //like getPhotos but without any filters
    }


    Photo::Data ASqlBackend::getPhoto(const Photo::Id& id)
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
            const std::optional<Photo::Sha256sum> sha256 = getSha256For(id);
            if (sha256)
                photoData.sha256Sum = *sha256;

            //load flags
            updateFlagsOn(photoData, id);

            // load group
            photoData.groupInfo = getGroupFor(id);
        }

        return photoData;
    }


    std::vector<Photo::Id> ASqlBackend::getPhotos(const std::vector<IFilter::Ptr>& filter)
    {
        const QString queryStr = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor.exec(queryStr, &query);
        auto result = fetch(query);

        return result;
    }


    int ASqlBackend::getPhotosCount(const std::vector<IFilter::Ptr>& filter)
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


    std::vector<PersonName> ASqlBackend::listPeople()
    {
        const QString findQuery = QString("SELECT id, name FROM %1")
                                    .arg( TAB_PEOPLE_NAMES );

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        std::vector<PersonName> result;
        const bool status = m_executor.exec(findQuery, &query);

        if (status)
        {
            if (m_dbHasSizeFeature)
                result.reserve(static_cast<std::size_t>(query.size()));

            while(query.next())
            {
                const int id = query.value(0).toInt();
                const QString name = query.value(1).toString();
                const Person::Id pid(id);

                result.emplace_back(pid, name);
            }
        }

        return result;
    }


    std::vector<PersonInfo> ASqlBackend::listPeople(const Photo::Id& ph_id )
    {
        const QString findQuery = QString("SELECT %1.id, %1.person_id, %1.location FROM %1 WHERE %1.photo_id = %2")
                                    .arg(TAB_PEOPLE)
                                    .arg(ph_id);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        std::vector<PersonInfo> result;
        const bool status = m_executor.exec(findQuery, &query);

        if (status)
        {
            if (m_dbHasSizeFeature)
                result.reserve(static_cast<std::size_t>(query.size()));

            while(query.next())
            {
                const int id_raw = query.value(0).toInt();
                const PersonInfo::Id id(id_raw);
                const Person::Id pid = query.isNull(1)?
                                           Person::Id():
                                           Person::Id(query.value(1).toInt());

                QRect location;

                if (query.isNull(2) == false)
                {
                    const QVariant location_raw = query.value(2);
                    const QStringList location_list = location_raw.toString().split(QRegExp("[ ,x]"));
                    location = QRect(location_list[0].toInt(),
                                     location_list[1].toInt(),
                                     location_list[2].toInt(),
                                     location_list[3].toInt());
                }

                result.emplace_back(id, pid, ph_id, location);
            }
        }

        return result;
    }


    /**
     * \brief get person name for given person id
     */
    PersonName ASqlBackend::person(const Person::Id& p_id)
    {
        const QString findQuery = QString("SELECT id, name FROM %1 WHERE %1.id = %2")
                                    .arg( TAB_PEOPLE_NAMES )
                                    .arg(p_id);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        PersonName result;
        const bool status = m_executor.exec(findQuery, &query);

        if (status && query.next())
        {
            const int id = query.value(0).toInt();
            const QString name = query.value(1).toString();
            const Person::Id pid(id);

            result = PersonName (pid, name);
        }

        return result;
    }


    Person::Id ASqlBackend::store(const PersonName& d)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        Person::Id id(d.id());
        bool status = false;

        InsertQueryData queryData(TAB_PEOPLE_NAMES);
        queryData.setColumns("name");
        queryData.setValues(d.name());

        QSqlQuery query;

        if (id.valid())  // id valid? override (update name)
        {
            UpdateQueryData updateQueryData(queryData);
            updateQueryData.addCondition("id", QString::number(id));
            query = getGenericQueryGenerator()->update(db, updateQueryData);

            status = m_executor.exec(query);

            if (query.numRowsAffected() == 0)   // any update?
                id = Person::Id();              // nope - error
        }
        else             // id invalid? add new person or nothing when already exists
        {
            const PersonName pn = person(d.name());

            if (pn.id().valid())
                id = pn.id();
            else
            {
                query = getGenericQueryGenerator()->insert(db, queryData);
                status = m_executor.exec(query);

                if (status)
                {
                    const QVariant vid  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
                    id = vid.toInt();
                }
            }
        }

        return id;
    }


    PersonInfo::Id ASqlBackend::store(const PersonInfo& fd)
    {
        assert(fd.ph_id);
        assert(fd.rect.isValid() || fd.p_id.valid() || fd.id.valid());  // if rect is invalid and person is invalid then at least id must be valid (removal operation)

        PersonInfo::Id result = fd.id;

        if (fd.id.valid() && fd.rect.isValid() == false && fd.p_id.valid() == false)
            dropPersonInfo(fd.id);
        else
        {
            PersonInfo to_store = fd;

            if (fd.id.valid() == false)
            {
                // determin if it is a new person, or we want to update existing one
                const auto existing_people = listPeople(fd.ph_id);

                for(const auto& person: existing_people)
                {
                    if (fd.rect.isValid() &&
                        person.rect == fd.rect)                 // same, valid rect
                    {
                        to_store.id = person.id;
                        break;
                    }
                    else if (person.p_id.valid()    &&
                             person.p_id == fd.p_id &&
                             person.rect.isValid() == false)    // same, valid person but no rect in db
                    {
                        to_store.id = person.id;
                        break;
                    }
                }
            }

            if (to_store.id.valid() && to_store.rect.isValid() == false && to_store.p_id.valid() == false)
                dropPersonInfo(fd.id);
            else
                result = storePerson(to_store);
        }

        return result;
    }


    void ASqlBackend::set(const Photo::Id& id, const QString& name, int value)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        UpdateQueryData updateData(TAB_GENERAL_FLAGS);
        updateData.setColumns("photo_id", "name", "value");
        updateData.setValues(id, name, value);
        updateData.addCondition("photo_id", QString::number(id));
        updateData.addCondition("name", name);

        updateOrInsert(updateData);
    }


    std::optional<int> ASqlBackend::get(const Photo::Id& id, const QString& name)
    {
        std::optional<int> result;

        const QString findQuery = QString("SELECT value FROM %1 WHERE photo_id = %2 AND name = '%3'")
                                    .arg(TAB_GENERAL_FLAGS)
                                    .arg(id)
                                    .arg(name);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        const bool status = m_executor.exec(findQuery, &query);

        if (status && query.next())
            result = query.value(0).toInt();

        return result;
    }


    std::vector<Photo::Id> ASqlBackend::markStagedAsReviewed()
    {
        auto filter = std::make_shared<FilterPhotosWithFlags>();
        filter->flags[Photo::FlagsE::StagingArea] = 1;

        const std::vector<Database::IFilter::Ptr> filters({filter});
        const std::vector<Photo::Id> staged = getPhotos(filters);

        if (staged.empty() == false)
        {
            const QString conversionQuery = QString("UPDATE %1 SET staging_area=0 WHERE staging_area=1")
                                                .arg(TAB_FLAGS);

            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            QSqlQuery query(db);

            m_executor.exec(conversionQuery, &query);

            emit photosMarkedAsReviewed(staged);
        }

        return staged;
    }


    /**
     * \brief validate database consistency
     */
    BackendStatus ASqlBackend::checkStructure()
    {
        BackendStatus status;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        Transaction transaction(m_tr_db);

        try
        {
            DB_ERROR_ON_FALSE(transaction.begin(), StatusCodes::TransactionFailed);

            //check tables existance
            for (const auto& table: tables)
                DB_ERR_ON_FALSE(ensureTableExists(table.second));

            QSqlQuery query(db);

            // table 'version' cannot be empty
            DB_ERROR_ON_FALSE(m_executor.exec("SELECT COUNT(*) FROM " TAB_VER ";", &query), StatusCodes::QueryFailed);

            DB_ERR_ON_FALSE(query.next());

            const QVariant rows = query.value(0);

            //insert first entry
            if (rows == 0)
                DB_ERR_ON_FALSE(m_executor.exec(QString("INSERT INTO " TAB_VER "(version) VALUES(%1);")
                                                         .arg(db_version), &query))
            else
                DB_ERR_ON_FALSE(checkDBVersion());

            DB_ERROR_ON_FALSE(transaction.commit(), StatusCodes::TransactionCommitFailed);
        }
        catch(const db_error& err)
        {
            m_logger->error(err.what());
            status = err.status();
        }

        return status;
    }


    /**
     * \brief check current db version and upgrade structures if required
     * \return operation status
     */
    BackendStatus ASqlBackend::checkDBVersion()
    {
        // WARNING: use raw SQL here. Do not use high level functions for data storage
        //          as they will save data using current algorithms which may
        //          break conversion chain.
        //
        //          Functions storing people for database in version n
        //          may store data in the different way than it was when db was in version n-3.

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        QSqlQuery query(db);

        BackendStatus status = m_executor.exec("SELECT version FROM " TAB_VER ";", &query);

        if (status)
            status = query.next()? StatusCodes::Ok: StatusCodes::QueryFailed;

        if (status)
        {
            const int v = query.value(0).toInt();

            switch (v)
            {
                case 0:

                case 1:             // update from 1 to 2
                {
                    // invalidate geometry table - from version 2 photo orientation will be considered
                    const QString cleanGeometry = QString("UPDATE %1 SET geometry_loaded = 0 WHERE geometry_loaded = 1").arg(TAB_FLAGS);
                    status = m_executor.exec(cleanGeometry, &query);
                    if (status == false)
                        break;

                } [[fallthrough]];

                case 2:             // update from 2 to 3
                {
                    // move all people from tags to new tables

                    // collect existing data
                    const QString find_query = QString("SELECT value, photo_id FROM %1 WHERE name=%2")
                                                    .arg(TAB_TAGS)
                                                    .arg(TagTypes::_People);

                    status = m_executor.exec(find_query, &query);

                    std::map<QString, Person::Id> name_to_id;
                    std::vector<std::pair<QString, Photo::Id>> old_data;

                    while(status && query.next())
                    {
                        const QString name = query.value(0).toString();
                        const Photo::Id ph_id(query.value(1).toInt());
                        old_data.emplace_back(name, ph_id);
                    }

                    for(const auto& d: old_data)
                    {
                        const QString& name = d.first;
                        const Photo::Id& ph_id = d.second;

                        // store name if unknown
                        auto it = name_to_id.find(name);

                        if (it == name_to_id.end())
                        {
                            InsertQueryData queryData(TAB_PEOPLE_NAMES);
                            queryData.setColumns("name");
                            queryData.setValues(name);

                            query = getGenericQueryGenerator()->insert(db, queryData);

                            status = m_executor.exec(query);

                            if (status)
                            {
                                const QVariant vid  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
                                const int id = vid.toInt();
                                auto ins_it = name_to_id.emplace(name, id);
                                it = ins_it.first;
                            }
                            else
                                status = Database::StatusCodes::MigrationFailed;
                        }

                        // store person
                        if (status)
                        {
                            InsertQueryData queryData(TAB_PEOPLE);
                            queryData.setColumns("photo_id", "person_id");
                            queryData.setValues(ph_id, it->second);

                            query = getGenericQueryGenerator()->insert(db, queryData);

                            status = m_executor.exec(query);

                            if (!status)
                                status = Database::StatusCodes::MigrationFailed;
                        }
                    }

                    if (status)
                    {
                        const QString drop_query = QString("DELETE FROM %1 WHERE name=%2")
                                                    .arg(TAB_TAGS)
                                                    .arg(TagTypes::_People);

                        QSqlQuery q(db);
                        m_executor.exec(drop_query, &query);
                    }

                    if (status == false)
                        break;
                } [[fallthrough]];

                case 3:
                {
                    // insert column with type for groups
                    const QString insertGroupType = QString("ALTER TABLE %1 ADD type INTEGER")
                                                        .arg(TAB_GROUPS);

                    status = m_executor.exec(insertGroupType, &query);
                    if (status == false)
                        break;

                    const QString setValue = QString("UPDATE %1 SET type = %2")
                                                .arg(TAB_GROUPS)
                                                .arg(static_cast<int>(Group::Type::Animation));

                    status = m_executor.exec(setValue, &query);
                    if (status == false)
                        break;

                } [[fallthrough]];

                case 4:             // current version, break updgrades chain
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
                status = m_executor.exec(queryStr, &query);
            }
        }

        return status;
    }


    /**
     * \brief get person name structure for person name
     * \param name person name as string
     * \return detailed name structure
     */
    PersonName ASqlBackend::person(const QString& name) const
    {
        PersonName result;

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        const QString s = QString("SELECT id, name FROM %1 WHERE name = :name").arg( TAB_PEOPLE_NAMES );
        m_executor.prepare(s, &query);
        query.bindValue(":name", name);

        m_executor.exec(query);

        if (query.next())
        {
            const Person::Id id( query.value(0).toInt() );
            const QString p_name( query.value(1).toString() );

            result = PersonName (id, p_name);
        }

        return result;
    }


    /**
     * \brief get people details for given people ids
     * \return vector of person details structure
     */
    std::vector<PersonInfo> ASqlBackend::listPeople(const std::vector<Photo::Id>& ids)
    {
        std::vector<PersonInfo> all_people;

        for(const Photo::Id& id: ids)
        {
            const auto people = listPeople(id);

            for (const PersonInfo& person: people)
                all_people.push_back(person);
        }

        return all_people;
    }


    /**
     * \brief store or update person details in database
     * \param fd person details
     * \return id assigned for person
     *
     * If fd contains valid id, person data will be updated. \n
     * If fd has no valid id, new person will be created.
     */
    PersonInfo::Id ASqlBackend::storePerson(const PersonInfo& fd)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        PersonInfo::Id id = fd.id;

        InsertQueryData queryData(TAB_PEOPLE);
        queryData.setColumns("photo_id");
        queryData.setValues(fd.ph_id);

        const QRect& face = fd.rect;
        const QString face_coords = face.isEmpty()?
                                        QString():
                                        QString("%1,%2 %3x%4")
                                            .arg(face.x())
                                            .arg(face.y())
                                            .arg(face.width())
                                            .arg(face.height());

        queryData.addColumn("location");
        queryData.addValue(face_coords);

        const QString person_id = fd.p_id.valid()?
                                    QString::number(fd.p_id):
                                    QString();

        queryData.addColumn("person_id");
        queryData.addValue(person_id);

        QSqlQuery query;

        if (id.valid())
        {
            UpdateQueryData updateQueryData(queryData);
            updateQueryData.addCondition("id", QString::number(id));
            query = getGenericQueryGenerator()->update(db, updateQueryData);
        }
        else
        {
            query = getGenericQueryGenerator()->insert(db, queryData);
        }

        const bool status = m_executor.exec(query);

        if (status && id.valid() == false)
        {
            const QVariant vid  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
            id = vid.toInt();
        }

        return id;
    }


    /**
     * \brief drop person details from database
     * \param id if of person to be dropped
     *
     * \todo no reaction on error
     */
    void ASqlBackend::dropPersonInfo(const PersonInfo::Id& id)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        const QString query = QString("DELETE FROM %1 WHERE id=%2")
                                .arg(TAB_PEOPLE)
                                .arg(id);

        QSqlQuery q(db);
        m_executor.exec(query, &q);
    }


    /**
     * \brief prepare sql statement for KEY creation
     */
    bool ASqlBackend::createKey(const TableDefinition::KeyDefinition& key, const QString& tableName, QSqlQuery& query) const
    {
        QString indexDesc;

        indexDesc += "CREATE " + key.type;
        indexDesc += " " + key.name + "_idx";
        indexDesc += " ON " + tableName;
        indexDesc += " " + key.def + ";";

        const bool status = m_executor.exec(indexDesc, &query);

        return status;
    }


    /**
     * \brief add tag to photo
     * \param tagValue tag value
     * \param photo_id id of photo
     * \param name_id id of tag name (tag type)
     * \param tag_id id of existing entry for value update.\n -1 if new entry is to be created.
     */
    bool ASqlBackend::store(const TagValue& tagValue, int photo_id, int name_id, int tag_id) const
    {
        //store tag values
        bool status = true;
        const Tag::ValueType type = tagValue.type();

        switch (type)
        {
            case Tag::ValueType::Empty:
                assert(!"Empty tag value!");
                break;

            case Tag::ValueType::Date:
            case Tag::ValueType::String:
            case Tag::ValueType::Time:
            case Tag::ValueType::Int:
            case Tag::ValueType::Color:
            {
                QSqlDatabase db = QSqlDatabase::database(m_connectionName);
                QSqlQuery query(db);

                const QString value = tagValue.rawValue();

                assert(value.isEmpty() == false);

                if (value.isEmpty() == false)
                {
                    InsertQueryData queryData(TAB_TAGS);
                    queryData.setColumns("value", "photo_id", "name");
                    queryData.setValues(value, photo_id, name_id);

                    if (tag_id == -1)
                        query = getGenericQueryGenerator()->insert(db, queryData);
                    else
                    {
                        UpdateQueryData updateQueryData(queryData);
                        updateQueryData.addCondition("id", QString::number(tag_id));
                        query = getGenericQueryGenerator()->update(db, updateQueryData);
                    }

                    const QMap<QString, QVariant> bound = query.boundValues();

                    QStringList binded_values;
                    for(QMap<QString, QVariant>::const_iterator it = bound.begin(); it != bound.end(); ++it)
                        binded_values.append(it.key() + " = " + it.value().toString());

                    const QString binded_values_msg = "Binded values: " + binded_values.join(", ");
                    m_logger->debug(binded_values_msg);

                    status = m_executor.exec(query);
                }

                break;
            }
        }

        return status;
    }


    /**
     * \brief create new entry for photo in database
     * \throws db_error when any error during communication with database occurs
     */
    void ASqlBackend::introduce(Photo::DataDelta& data)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        //store path and date
        Photo::Id id = data.getId();
        assert(id.valid() == false);

        InsertQueryData insertData(TAB_PHOTOS);

        insertData.setColumns("path", "store_date");
        insertData.setValues(data.get<Photo::Field::Path>(), InsertQueryData::Value::CurrentTime);
        insertData.setColumns("id");
        insertData.setValues(InsertQueryData::Value::Null);

        QSqlQuery query = getGenericQueryGenerator()->insert(db, insertData);

        DB_ERR_ON_FALSE(m_executor.exec(query));

        // update id
        // Get Id from database after insert

        QVariant photo_id  = query.lastInsertId(); //TODO: WARNING: may not work (http://qt-project.org/doc/qt-5.1/qtsql/qsqlquery.html#lastInsertId)
        DB_ERR_ON_FALSE(photo_id.isValid());

        id = Photo::Id(photo_id.toInt());

        //make sure id is set
        DB_ERR_ON_FALSE(id.valid());

        assert(data.getId().valid() == false || data.getId() == id);
        data.setId(id);

        DB_ERR_ON_FALSE(storeData(data));
    }


    /**
     * \brief store photo data
     */
    bool ASqlBackend::storeData(const Photo::DataDelta& data)
    {
        const Photo::Data currentStateOfPhoto = getPhoto(data.getId());

        assert(data.getId());

        bool status = true;

        //store used tags
        if (data.has(Photo::Field::Tags))
        {
            const Tag::TagsList& tags = data.get<Photo::Field::Tags>();

            status = storeTags(data.getId(), tags);
        }

        if (status && data.has(Photo::Field::Geometry))
        {
            const QSize& geometry = data.get<Photo::Field::Geometry>();
            status = storeGeometryFor(data.getId(), geometry);
        }

        if (status && data.has(Photo::Field::Checksum))
        {
            const Photo::Sha256sum& checksum = data.get<Photo::Field::Checksum>();
            status = storeSha256(data.getId(), checksum);
        }

        if (status && data.has(Photo::Field::Flags))
        {
            const Photo::FlagValues& flags = data.get<Photo::Field::Flags>();
            status = storeFlags(data.getId(), flags);
        }

        if (status && data.has(Photo::Field::GroupInfo))
        {
            const GroupInfo& groupInfo = data.get<Photo::Field::GroupInfo>();
            status = storeGroup(data.getId(), groupInfo);
        }

        photoChangeLogOperator()->storeDifference(currentStateOfPhoto, data);

        return status;
    }

    /**
     * \brief store photo's dimensions
     * \return false on error
     */
    bool ASqlBackend::storeGeometryFor(const Photo::Id& photo_id, const QSize& geometry) const
    {
        UpdateQueryData data(TAB_GEOMETRY);
        data.addCondition("photo_id", QString::number(photo_id));
        data.setColumns("photo_id", "width", "height");
        data.setValues(QString::number(photo_id), QString::number(geometry.width()), QString::number(geometry.height()) );

        const bool status = updateOrInsert(data);

        return status;
    }


    /**
     * \brief store photo's sha256 checksum
     * \return false on error
     */
    bool ASqlBackend::storeSha256(int photo_id, const Photo::Sha256sum& sha256) const
    {
        UpdateQueryData data(TAB_SHA256SUMS);
        data.addCondition("photo_id", QString::number(photo_id));
        data.setColumns("photo_id", "sha256");
        data.setValues(QString::number(photo_id), sha256.constData());

        const bool status = updateOrInsert(data);

        return status;
    }

    /**
     * \brief store photo's tags in database
     * \return false on error
     */
    bool ASqlBackend::storeTags(int photo_id, const Tag::TagsList& tagsList) const
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
            std::vector<int> currentIds;

            while (query.next())
            {
                const QVariant idRaw = query.value(0);
                const int id = idRaw.toInt();

                currentIds.push_back(id);
            }

            // difference between current set in db and new set of tags
            const int currentIdsSize = static_cast<int>( currentIds.size() );
            const int tagsListSize   = static_cast<int>( tagsList.size() );
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

            for (auto it = tagsList.begin(); status && it != tagsList.end(); ++it, counter++)
            {
                const TagValue& value = it->second;
                const int name = it->first.getTag();
                const int tag_id = counter < currentIds.size()? currentIds[counter]: -1;  // try to override ids of tags already stored

                status = store(value, photo_id, name, tag_id);
            }

        }

        return status;
    }


    /**
     * \brief store photo's flags
     * \return false on error
     */
    bool ASqlBackend::storeFlags(const Photo::Id& id, const Photo::FlagValues& flags) const
    {
        auto get_flag = [&flags](Photo::FlagsE flag)
        {
            auto it = flags.find(flag);

            const int result = it != flags.end()? it->second : 0;
            return result;
        };

        UpdateQueryData queryInfo(TAB_FLAGS);
        queryInfo.addCondition("photo_id", QString::number(id));
        queryInfo.setColumns("photo_id", "staging_area", "tags_loaded", "sha256_loaded", "thumbnail_loaded", FLAG_GEOM_LOADED);
        queryInfo.setValues(QString::number(id),
                            get_flag(Photo::FlagsE::StagingArea),
                            get_flag(Photo::FlagsE::ExifLoaded),
                            get_flag(Photo::FlagsE::Sha256Loaded),
                            get_flag(Photo::FlagsE::ThumbnailLoaded),
                            get_flag(Photo::FlagsE::GeometryLoaded)
        );

        const bool status = updateOrInsert(queryInfo);

        return status;
    }


    /**
     * \brief store photo's group details
     * \return false on error
     */
    bool ASqlBackend::storeGroup(const Photo::Id& id, const GroupInfo& groupInfo) const
    {
        bool status = true;

        if (groupInfo.group_id.valid())
            switch (groupInfo.role)
            {
                case GroupInfo::Member:
                {
                    UpdateQueryData queryInfo(TAB_GROUPS_MEMBERS);
                    queryInfo.addCondition("photo_id", QString::number(id));
                    queryInfo.setColumns("group_id", "photo_id");
                    queryInfo.setValues(QString::number(groupInfo.group_id),
                                        QString::number(id)
                    );

                    status = updateOrInsert(queryInfo);
                    break;
                }

                case GroupInfo::Representative:
                    // do nothing - Information about representative was stored during group creation.
                    // Is it nice? What if photo's role changed? TODO: rethink
                    break;

                case GroupInfo::None:
                    // do nothing. Do doubts here ;]
                    break;
            }
        else
        {
            // TODO: remove media from group
        }

        return status;
    }


    /**
     * \brief insert set of photos to database
     * \param data_set vector of photo details to be stored
     * \return true on success.
     */
    bool ASqlBackend::insert(std::vector<Photo::DataDelta>& data_set)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        Transaction transaction(m_tr_db);

        bool status = true;

        try
        {
            DB_ERR_ON_FALSE(transaction.begin());

            for(Photo::DataDelta& data: data_set)
                introduce(data);

            DB_ERR_ON_FALSE(transaction.commit());
        }
        catch(const db_error& error)
        {
            m_logger->error(error.what());
            status = false;
        }

        return status;
    }


    /**
     * \brief get all tags assigned to photo
     * \param photoId id of photo
     * \return list of tags for given photo
     */
    Tag::TagsList ASqlBackend::getTagsFor(const Photo::Id& photoId) const
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
            const TagTypes tagNameType = static_cast<TagTypes>( query.value(1).toInt() );
            const QVariant value = query.value(2);
            const TagTypeInfo tagName(tagNameType);

            // storing routine doesn't store empty tags (see store() for tags)
            assert(value.isValid() && value.isNull() == false);
            if (value.isValid() == false || value.isNull())
                continue;

            const QString raw_value = value.toString();
            const TagValue tagValue = TagValue::fromRaw(raw_value, BaseTags::getType(tagName.getTag()));

            tagData[tagName] = tagValue;
        }

        return tagData;
    }


    /**
     * \brief read photo's geometry
     * \param id photo id
     * \return photo size
     */
    QSize ASqlBackend::getGeometryFor(const Photo::Id& id) const
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


    /**
     * \brief read photo's checksum
     * \param id photo id
     * \return photo's checksum
     */
    std::optional<Photo::Sha256sum> ASqlBackend::getSha256For(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT sha256 FROM %1 WHERE %1.photo_id = '%2'");

        queryStr = queryStr.arg(TAB_SHA256SUMS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        std::optional<Photo::Sha256sum> result;
        if(status && query.next())
        {
            const QVariant variant = query.value(0);

            result = variant.toString().toLatin1();
        }

        return result;
    }


    /**
     * \brief read details about group
     * \param id photo id
     * \return group details
     */
    GroupInfo ASqlBackend::getGroupFor(const Photo::Id& id) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);
        QString queryStr = QString("SELECT %1.id, %1.representative_id, %2.photo_id FROM %1 "
                                   "JOIN %2 ON (%1.id = %2.group_id) "
                                   "WHERE (%1.representative_id = %3 OR %2.photo_id = %3)"
        );

        queryStr = queryStr.arg(TAB_GROUPS);
        queryStr = queryStr.arg(TAB_GROUPS_MEMBERS);
        queryStr = queryStr.arg(id.value());

        const bool status = m_executor.exec(queryStr, &query);

        GroupInfo result;
        for(query.next(); status && query.isValid(); query.next())
        {
            const QVariant groupVariant = query.value(0);
            const QVariant representativeVariant = query.value(1);
            const QVariant memberVariant = query.value(2);

            const int groupId = groupVariant.toInt();
            const int representativeId = representativeVariant.toInt();
            const int memberId = memberVariant.toInt();

            const Group::Id gid(groupId);

            if (id == representativeId)
            {
                result = GroupInfo(gid, GroupInfo::Representative);
                break;
            }
            else if (id == memberId)
            {
                result = GroupInfo(gid, GroupInfo::Member);
                break;
            }
        }

        return result;
    }


    /**
     * \brief read flags for photo
     * \param photoData input parameter - photo details
     * \param id photo id
     *
     * Method will modify flags in \p photoData parameter. Other entries will not be touched
     */
    void ASqlBackend::updateFlagsOn(Photo::Data& photoData, const Photo::Id& id) const
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

    /**
     * \brief read photo path
     * \param id photo id
     * \return path to photo
     */
    QString ASqlBackend::getPathFor(const Photo::Id& id) const
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


    /**
     * \brief collect photo ids SELECTed by SQL query
     * \param query SQL SELECT query which returns photo ids
     * \return unique list of photo ids
     */
    std::vector<Photo::Id> ASqlBackend::fetch(QSqlQuery& query) const
    {
        std::vector<Photo::Id> collection;

        while (query.next())
        {
            const Photo::Id id(query.value("photos.id").toInt());

            collection.push_back(id);
        }

        // remove duplicates which may appear when query is complex
        std::sort(collection.begin(), collection.end());
        auto last = std::unique(collection.begin(), collection.end());
        collection.erase(last, collection.end());

        return collection;
    }


    /**
     * \brief check if \param id is a valid photo id.
     */
    bool ASqlBackend::doesPhotoExist(const Photo::Id& id) const
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


    /**
     * \brief insert data to database or upgrade existing entries.
     * \param queryInfo data to be inserted with rules when to update.
     * \return true on success
     */
    bool ASqlBackend::updateOrInsert(const UpdateQueryData& queryInfo) const
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        QSqlQuery query = getGenericQueryGenerator()->update(db, queryInfo);

        bool status = m_executor.exec(query);

        if (status)
        {
            const int affected_rows = query.numRowsAffected();

            if (affected_rows == 0)
            {
                query = getGenericQueryGenerator()->insert(db, queryInfo);
                status = m_executor.exec(query);
            }
        }

        return status;
    }

}
