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
#include <core/generic_concepts.hpp>
#include <database/ibackend.hpp>
#include <database/general_flags.hpp>

#include "isql_query_executor.hpp"
#include "query_structs.hpp"
#include "sql_filter_query_generator.hpp"
#include "tables.hpp"
#include "utils.hpp"


namespace Database
{

    namespace {
        std::map<Tag::Types, const char *> namesForJoins =
        {
            { Tag::Types::Event,    "event_tag"    },
            { Tag::Types::Place,    "place_tag"    },
            { Tag::Types::Date,     "date_tag"     },
            { Tag::Types::Time,     "time_tag"     },
            { Tag::Types::Rating,   "rating_tag"   },
            { Tag::Types::Category, "category_tag" },
        };

        template<typename T, std::size_t... Is>
        void forEachItem(T& tp, auto op, std::index_sequence<Is...>)
        {
            (op(std::get<Is>(tp), Is), ...);
        }

        template<typename T, typename = void>
        struct is_id_type : std::false_type {};

        template<typename T>
        struct is_id_type<T, std::void_t<typename T::type, typename T::tag>>: std::true_type {};

        template<typename vType>
        constexpr bool is_id_type_v = is_id_type<vType>::value;

        template<typename... Args>
        auto readValues(const QSqlQuery& query)
        {
            std::tuple<Args...> values;

            forEachItem(
                values,
                [&query](auto& v, int i)
                {
                    using vType = std::decay_t<decltype(v)>;
                    const QVariant value = query.value(i);

                    // all Id based types
                    if constexpr (is_id_type_v<vType>)
                    {
                        assert(value.canConvert<typename vType::type>());
                        v = value.value<typename vType::type>();
                    }
                    else
                    {
                        assert(value.canConvert<vType>());
                        v = value.value<vType>();
                    }
                },
                std::make_integer_sequence<std::size_t, sizeof...(Args)>()
            );

            return values;
        }
    }

    PhotoOperator::PhotoOperator(const QString& connection,
                                 ISqlQueryExecutor* executor,
                                 const IGenericSqlQueryGenerator& queryGenerator,
                                 ILogger* logger,
                                 IBackend* backend,
                                 NotificationsAccumulator& notificationsAccumulator):
        m_connectionName(connection),
        m_executor(executor),
        m_queryGenerator(queryGenerator),
        m_logger(logger),
        m_backend(backend),
        m_notifications(notificationsAccumulator)
    {

    }


    bool PhotoOperator::removePhoto(const Photo::Id& id)
    {
        FilterPhotosWithId id_filter;
        id_filter.filter = id;

        return removePhotos(id_filter);
    }


    bool PhotoOperator::removePhotos(const Filter& filter)
    {
        const QString filterQuery = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        auto tr = m_backend->openTransaction();

        //collect ids of photos to be dropped and mark them
        std::vector<Photo::Id> ids;
        bool status = m_executor->exec(filterQuery, &query);

        if (status)
        {
            while(query.next())
            {
                const Photo::Id id(query.value(0));

                // mark photo as deleted
                m_backend->setBits(id, CommonGeneralFlags::State, static_cast<int>(CommonGeneralFlags::StateType::Delete));
                ids.push_back(id);
            }
        }

        if (status)
            m_notifications.photosRemoved(ids);
        else
            tr->abort();

        return status;
    }


    std::vector<Photo::Id> PhotoOperator::onPhotos(const Filter& filters, const Action& action)
    {
        SortingContext context;
        processAction(context, action);

        const QString filtersQuery = SqlFilterQueryGenerator().generate(filters);
        const QString actionQuery =
            QString("SELECT photos.id FROM (%2) "
                    "%3 "
                    "WHERE %2.id IN (%1) ORDER BY %4")
            .arg(filtersQuery)
            .arg(TAB_PHOTOS)
            .arg(context.joins.join(" "))
            .arg(context.sortOrder.join(", "));

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor->exec(actionQuery, &query);
        auto result = fetch(query);

        return result;
    }


    std::vector<Photo::Id> PhotoOperator::getPhotos(const Filter& filter)
    {
        const QString queryStr = SqlFilterQueryGenerator().generate(filter);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor->exec(queryStr, &query);
        auto result = fetch(query);

        return result;
    }


    std::vector<Photo::DataDelta> PhotoOperator::fetchData(const Filter& filter, const std::set<Photo::Field>& fields)
    {
        assert(fields.empty() == false);

        std::unordered_map<Photo::Id, Photo::DataDelta> accumulator;

        // collect all matching ids
        const auto ids = getPhotos(filter);

        for (const auto id: ids)
        {
            Photo::DataDelta delta(id);

            // preset all requiered fields
            for(auto field: fields)
                delta.clear(field);

            accumulator[id] = delta;
        }

        if (fields.contains(Photo::Field::Path))
        {
            const auto paths = getPaths(filter);
            for (const auto& [id, path]: paths)
                accumulator[id].insert<Photo::Field::Path>(path);
        }

        if (fields.contains(Photo::Field::Tags))
        {
            const auto photos_tags = getTags(filter);
            for (const auto& [id, tags]: photos_tags)
                accumulator[id].insert<Photo::Field::Tags>(tags);
        }

        if (fields.contains(Photo::Field::Flags))
        {
            const auto photos_flags = getFlags(filter);
            for (const auto& [id, flags]: photos_flags)
                accumulator[id].insert<Photo::Field::Flags>(flags);
        }

        if (fields.contains(Photo::Field::Geometry))
        {
            const auto photos_geometry = getGeometry(filter);
            for (const auto& [id, geometry]: photos_geometry)
                accumulator[id].insert<Photo::Field::Geometry>(geometry);
        }

        if (fields.contains(Photo::Field::GroupInfo))
        {
            const auto photos_group = getGroups(filter);
            for (const auto& [id, group]: photos_group)
                accumulator[id].insert<Photo::Field::GroupInfo>(group);
        }

        if (fields.contains(Photo::Field::PHash))
        {
            const auto photos_phashes = getPHashes(filter);
            for (const auto& [id, phash]: photos_phashes)
                accumulator[id].insert<Photo::Field::PHash>(phash);
        }

        if (fields.contains(Photo::Field::People))
        {
            const auto photos_people = getPeople(filter);
            for (const auto& [id, people]: photos_people)
                accumulator[id].insert<Photo::Field::People>(people);
        }

        std::vector<Photo::DataDelta> deltas;
        deltas.reserve(accumulator.size());

        for (auto& [id, delta]: accumulator)
            deltas.push_back(delta);

        return deltas;
    }


    void PhotoOperator::setPHash(const Photo::Id& id, const Photo::PHashT& phash)
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);

        InsertQueryData insertQueryData(TAB_PHASHES);
        insertQueryData.setColumns("photo_id", "hash");
        insertQueryData.setValues(id.value(), phash.variant());

        QSqlQuery query;
        if (hasPHash(id))
        {
            UpdateQueryData updateQueryData(insertQueryData);
            updateQueryData.addCondition("photo_id", QString::number(id.value()));

            query = m_queryGenerator.update(db, updateQueryData);
        }
        else
            query = m_queryGenerator.insert(db, insertQueryData);

        m_executor->exec(query);
    }


    std::optional<Photo::PHashT> PhotoOperator::getPHash(const Photo::Id& id)
    {
        const QString queryStr = QString("SELECT hash FROM %1 WHERE photo_id=%2")
            .arg(TAB_PHASHES)
            .arg(id.value());

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor->exec(queryStr, &query);

        std::optional<Photo::PHashT> result;
        if (query.next())
        {
            const Photo::PHashT phash(query.value("hash").toLongLong());

            result = phash;
        }

        return result;
    }


    bool PhotoOperator::hasPHash(const Photo::Id& id)
    {
        const QString queryStr = QString("SELECT COUNT(photo_id) FROM %1 WHERE photo_id=%2")
            .arg(TAB_PHASHES)
            .arg(id.value());

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor->exec(queryStr, &query);

        const int count = query.next()? query.value(0).toInt() : 0;

        return count > 0;
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
            const QVariant idVar = query.value(0);   // assumption that id will be in 1. column
            assert(idVar.isValid());
            assert(idVar.canConvert<int>());

            const Photo::Id id(idVar);

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


    void PhotoOperator::processAction(ActionContext& context, const Action& action) const
    {
        if (auto sort_by_tag = std::get_if<Actions::SortByTag>(&action))
        {
            auto it = namesForJoins.find(sort_by_tag->tag);

            if (it != namesForJoins.end())
            {
                const char* joinName = it->second;

                context.joins.append(QString("LEFT JOIN %1 %4 ON (%2.id = %4.photo_id AND %4.name = %3)")
                    .arg(TAB_TAGS)
                    .arg(TAB_PHOTOS)
                    .arg(sort_by_tag->tag)
                    .arg(joinName));

                context.sortOrder.append(QString("%2.value %1")
                    .arg(sort_by_tag->sort_order == Qt::AscendingOrder? "ASC": "DESC")
                    .arg(joinName));
            }
        }
        else if (auto group_action = std::get_if<Actions::GroupAction>(&action))
        {
            for (const auto& sub_action: group_action->actions)
                processAction(context, sub_action);
        }
        else if (auto sort = std::get_if<Actions::Sort>(&action))
        {
            switch(sort->by)
            {
                case Actions::Sort::By::PHash:
                {
                    context.joins.append(QString("LEFT JOIN %1 ON (%2.id = %1.photo_id)")
                        .arg(TAB_PHASHES)
                        .arg(TAB_PHOTOS));

                    context.sortOrder.append(QString("%2.hash %1")
                        .arg(sort->order == Qt::AscendingOrder? "ASC": "DESC")
                        .arg(TAB_PHASHES));

                    break;
                }

                case Actions::Sort::By::Timestamp:
                {
                    const Actions::SortByTag byDate(Tag::Types::Date, sort->order);
                    const Actions::SortByTag byTime(Tag::Types::Time, sort->order);
                    processAction(context, byDate);
                    processAction(context, byTime);
                    break;
                }

                case Actions::Sort::By::ID:
                    context.sortOrder.append(QString("%1.id %2")
                        .arg(TAB_PHOTOS)
                        .arg(sort->order == Qt::AscendingOrder? "ASC": "DESC"));
                    break;
            }
        }
        else
            assert(!"Unknown action");
    }


    std::unordered_map<Photo::Id, QString> PhotoOperator::getPaths(const Filter& filter) const
    {
        const QString query = QString("SELECT id, path FROM %1").arg(TAB_PHOTOS);
        const auto pathsOfMatchingPhotos = getAny<QString>(filter, query, [](const QSqlQuery& sqlQuery)
        {
            const auto [id, path] = readValues<Photo::Id, QString>(sqlQuery);
            return std::tuple{id, path};
        },
        "id");

        return pathsOfMatchingPhotos;
    }


    std::unordered_map<Photo::Id, Tag::TagsList> PhotoOperator::getTags(const Filter& filter) const
    {
        const QString query = QString("SELECT photo_id, name, value FROM %1").arg(TAB_TAGS);
        const auto tagsOfMatchingPhotos = getAny<Tag::TagsList>(filter, query, [](const QSqlQuery& sqlQuery)
        {
            const auto [id, tagTypeI, tagValue] = readValues<Photo::Id, int, QString>(sqlQuery);
            const Tag::Types tagType = static_cast<Tag::Types>(tagTypeI);

            return std::tuple{id, std::pair{tagType, TagValue::fromRaw(tagValue, BaseTags::getType(tagType)) }};
        });

        return tagsOfMatchingPhotos;
    }


    std::unordered_map<Photo::Id, Photo::FlagValues> PhotoOperator::getFlags(const Filter& filter) const
    {
        const QString query = QString("SELECT photo_id, staging_area, tags_loaded, geometry_loaded FROM %1").arg(TAB_FLAGS);
        const auto flagsOfMatchingPhotos = getAny<Photo::FlagValues>(filter, query, [](const QSqlQuery& sqlQuery)
        {
            const auto [id, staging_area, tags_loaded, geometry_loaded] = readValues<Photo::Id, int, int, int>(sqlQuery);
            const Photo::FlagValues flags{
                {Photo::FlagsE::StagingArea, staging_area},
                {Photo::FlagsE::ExifLoaded, tags_loaded},
                {Photo::FlagsE::GeometryLoaded, geometry_loaded}
            };

            return std::tuple{id, flags};
        });

        return flagsOfMatchingPhotos;
    }


    std::unordered_map<Photo::Id, QSize> PhotoOperator::getGeometry(const Filter& filter) const
    {
        const QString query = QString("SELECT photo_id, width, height FROM %1").arg(TAB_GEOMETRY);
        const auto geometryOfMatchingPhotos = getAny<QSize>(filter, query, [](const QSqlQuery& sqlQuery)
        {
            const auto [id, width, height] = readValues<Photo::Id, int, int>(sqlQuery);
            return std::tuple{id, QSize(width, height)};
        });

        return geometryOfMatchingPhotos;
    }


    std::unordered_map<Photo::Id, GroupInfo> PhotoOperator::getGroups(const Filter& filter) const
    {
        const QString membersQuery = QString("SELECT group_id, photo_id FROM %1").arg(TAB_GROUPS_MEMBERS);

        const auto groupsMembersOfMatchingPhotos = getAny<GroupInfo>(filter, membersQuery, [](const QSqlQuery& sqlQuery)
        {
            const auto [g_id, p_id] = readValues<Group::Id, Photo::Id>(sqlQuery);
            return std::tuple{p_id, GroupInfo(g_id, GroupInfo::Member)};
        });

        const QString representativesQuery = QString("SELECT id, representative_id FROM %1").arg(TAB_GROUPS);

        const auto groupsRepresentativesOfMatchingPhotos = getAny<GroupInfo>(filter, representativesQuery, [](const QSqlQuery& sqlQuery)
        {
            const auto [g_id, p_id] = readValues<Group::Id, Photo::Id>(sqlQuery);
            return std::tuple{p_id, GroupInfo(g_id, GroupInfo::Representative)};
        },
        "representative_id");

        std::unordered_map<Photo::Id, GroupInfo> groupsOfMatchingPhotos;
        groupsOfMatchingPhotos.insert(groupsMembersOfMatchingPhotos.begin(), groupsMembersOfMatchingPhotos.end());
        groupsOfMatchingPhotos.insert(groupsRepresentativesOfMatchingPhotos.begin(), groupsRepresentativesOfMatchingPhotos.end());

        return groupsOfMatchingPhotos;
    }


    std::unordered_map<Photo::Id, Photo::PHashT> PhotoOperator::getPHashes(const Filter& filter) const
    {
        const QString query = QString("SELECT photo_id, hash FROM %1").arg(TAB_PHASHES);
        const auto hashesOfMatchingPhotos = getAny<Photo::PHashT>(filter, query, [](const QSqlQuery& sqlQuery)
        {
            const auto [id, hash] = readValues<Photo::Id, Photo::PHashT>(sqlQuery);
            return std::tuple{id, hash};
        });

        return hashesOfMatchingPhotos;
    }


    std::unordered_map<Photo::Id, std::vector<PersonFullInfo>> PhotoOperator::getPeople(const Filter& filter) const
    {
        const QString query = QString("SELECT %1.id, %1.photo_id, %1.person_id, %1.fingerprint_id, %1.location, %2.name, %3.fingerprint FROM %1 "
                                      "JOIN %2 ON (%2.id = %1.person_id) "
                                      "JOIN %3 ON (%3.id = %1.fingerprint_id)")
                                .arg(TAB_PEOPLE)
                                .arg(TAB_PEOPLE_NAMES)
                                .arg(TAB_FACES_FINGERPRINTS);

        const auto peopleOfMatchingPhotos = getAny<std::vector<PersonFullInfo>>(filter, query, [](const QSqlQuery& sqlQuery)
        {
            const auto [personInfo_id, photo_id, person_id, fingerprint_id, location, name, fingerprint] = readValues<PersonInfo::Id, Photo::Id, Person::Id, PersonFingerprint::Id, QString, QString, QByteArray>(sqlQuery);
            const auto locationRect = decodeFaceLocation(location);
            const auto personFingerprint = decodeFingerprint(fingerprint);

            return std::tuple{photo_id, PersonFullInfo(PersonName(person_id, name), PersonFingerprint(fingerprint_id, personFingerprint), locationRect, personInfo_id)};
        });

        return peopleOfMatchingPhotos;
    }


    template<typename T, typename C>
    std::unordered_map<Photo::Id, T> PhotoOperator::getAny(const Filter& filter, const QString& queryStr, C op, const QString& filterColumn) const
    {
        const QString filterQuery = SqlFilterQueryGenerator().generate(filter);
        const QString finalQueryStr = QString("%1 WHERE %2 IN (%3)").arg(queryStr).arg(filterColumn).arg(filterQuery);

        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        QSqlQuery query(db);

        m_executor->exec(finalQueryStr, &query);

        std::unordered_map<Photo::Id, T> anyForPhoto;

        while (query.next())
        {
            const auto [id, any] = op(query);

            if constexpr (std::is_same_v<T, std::decay_t<decltype(any)>>)
                anyForPhoto.emplace(id, any);
            else if constexpr (map_type<T>)
                anyForPhoto[id].insert(any);
            else if constexpr (is_std_vector_v<T>)
                anyForPhoto[id].push_back(any);
            else
                static_assert(always_false<T>::value, "Unexpected type");
        }

        return anyForPhoto;
    }
}
