
#include <QFileInfo>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <core/utils.hpp>
#include <core/containers_utils.hpp>
#include "memory_backend.hpp"
#include "database/general_flags.hpp"
#include "database/notifications_accumulator.hpp"
#include "database/project_info.hpp"
#include "database/photo_utils.hpp"
#include "database/transaction_wrapper.hpp"


using namespace boost::multi_index;

namespace Database
{

    struct pi_id_tag {};
    struct pi_ph_id_tag {};

    using PeopleContainer = boost::multi_index_container<
        PersonInfo,
        indexed_by<
            ordered_unique<tag<pi_id_tag>, BOOST_MULTI_INDEX_MEMBER(PersonInfo, PersonInfo::Id, id)>,
            ordered_non_unique<tag<pi_ph_id_tag>, BOOST_MULTI_INDEX_MEMBER(PersonInfo, Photo::Id, ph_id)>
        >
    >;

    struct MemoryBackend::DB
    {
        std::map<Photo::Id, Flags> m_flags;
        std::map<Group::Id, GroupData> m_groups;
        std::set<StoregeDelta, IdComparer<StoregeDelta, Photo::Id>> m_photos;
        std::set<PersonName, IdComparer<PersonName, Person::Id>> m_peopleNames;
        PeopleContainer m_peopleInfo;
        std::vector<LogEntry> m_logEntries;
        std::map<std::pair<Photo::Id, QString>, QByteArray> m_blobs;

        int m_nextPhotoId = 0;
        int m_nextPersonName = 0;
        int m_nextGroup = 0;
        int m_nextPersonInfo = 0;
    };

    namespace
    {
        template<typename T>
        bool compare(const T& lhs, const T& rhs, Qt::SortOrder order)
        {
            return order == Qt::AscendingOrder? lhs < rhs: rhs < lhs;
        }

        int tristate_compare(const Photo::Data& lhs, const Photo::Data& rhs, const Tag::Types& tagType, Qt::SortOrder order)
        {
            const auto l_it = lhs.tags.find(tagType);
            const auto r_it = rhs.tags.find(tagType);
            const auto l_tag = l_it == lhs.tags.end()? TagValue(): l_it->second;
            const auto r_tag = r_it == rhs.tags.end()? TagValue(): r_it->second;

            const bool is_less = compare(l_tag, r_tag, order);
            const bool is_greater = compare(r_tag, l_tag, order);

            return (is_less? -1: 0) + (is_greater? 1: 0);
        }

        std::vector<MemoryBackend::StoregeDelta> filterPhotos(const std::vector<MemoryBackend::StoregeDelta>& photos,
                                                const Database::MemoryBackend::DB& db,
                                                const Database::Filter& dbFilter)
        {
            std::vector<MemoryBackend::StoregeDelta> result = photos;

            std::visit([&result, &db](auto&& filter)
            {
                using T = std::decay_t<decltype(filter)>;
                if constexpr (std::is_same_v<T, Database::FilterSimilarPhotos>)
                {
                    result.erase(std::remove_if(result.begin(), result.end(), [](const MemoryBackend::StoregeDelta& photo) {
                        return !photo.get<Photo::Field::PHash>().valid();
                    }), result.end());

                    std::sort(result.begin(), result.end(), [](const MemoryBackend::StoregeDelta& lhs, const MemoryBackend::StoregeDelta& rhs) {
                        return lhs.get<Photo::Field::PHash>() < rhs.get<Photo::Field::PHash>();
                    });

                    result.erase(remove_unique(result.begin(), result.end(), [](const MemoryBackend::StoregeDelta& lhs, const MemoryBackend::StoregeDelta& rhs) {
                        return lhs.get<Photo::Field::PHash>() == rhs.get<Photo::Field::PHash>();
                    }), result.end());
                }
                else if constexpr (std::is_same_v<T, Database::FilterPhotosWithPHash>)
                {
                    result.erase(std::remove_if(result.begin(), result.end(), [](const MemoryBackend::StoregeDelta& photo) {
                        return !photo.get<Photo::Field::PHash>().valid();
                    }), result.end());
                }
                else if constexpr (std::is_same_v<T, Database::FilterPhotosWithGeneralFlag>)
                {
                    result.erase(std::remove_if(result.begin(), result.end(), [&filter, &db](const MemoryBackend::StoregeDelta& photo) {

                        int value = 0;
                        auto it = db.m_flags.find(photo.getId());

                        if (it != db.m_flags.end())             // if no flags for given photo, continue with value == 0
                        {
                            const auto& flagsMap = it->second;
                            auto itm = flagsMap.find(filter.name);

                            value = itm == flagsMap.end()? 0: itm->second;    // if no flag value for given flag, continue with value == 0
                        }

                        return filter.mode == Database::FilterPhotosWithGeneralFlag::Mode::Exact?
                            value != filter.value:
                            (value & filter.value) != filter.value;

                    }), result.end());
                }
                else if constexpr (std::is_same_v<T, Database::FilterFaceAnalysisStatus>)
                {
                    result.erase(std::remove_if(result.begin(), result.end(), [&filter, &db](const MemoryBackend::StoregeDelta& photo) {
                        bool performed = false;

                        const auto ph_id = photo.getId();
                        const auto it = db.m_flags.find(ph_id);

                        // 'analyzed' flag set?
                        if (it != db.m_flags.end())
                        {
                            const auto f_it =  it->second.find(CommonGeneralFlags::FacesAnalysisState);

                            if (f_it != it->second.end() && static_cast<CommonGeneralFlags::FacesAnalysisType>(f_it->second) == CommonGeneralFlags::FacesAnalysisType::AnalysedAndNotFound)
                                performed = true;
                        }

                        // any people?      TODO: use contains() when boost 1.78 is available on github actions
                        const auto& c = get<pi_ph_id_tag>(db.m_peopleInfo);
                        const auto c_it = c.find(ph_id);
                        performed = c_it != c.end();

                       return (filter.status == Database::FilterFaceAnalysisStatus::Performed && !performed) ||
                              (filter.status == Database::FilterFaceAnalysisStatus::NotPerformed && performed);
                    }), result.end());
                }

            }, dbFilter);

            return result;
        }


        class Transaction
        {
            public:
                Transaction(std::unique_ptr<MemoryBackend::DB>& db, NotificationsAccumulator& notifications)
                    : m_savedState(std::make_unique<MemoryBackend::DB>(*db))
                    , m_dbRef(db)
                    , m_notifications(notifications)
                {

                }

                void rollback()
                {
                    m_dbRef.swap(m_savedState);
                    m_notifications.ignoreChanges();
                }

                void commit()
                {
                    m_notifications.fireChanges();
                }

            private:
                std::unique_ptr<MemoryBackend::DB> m_savedState;
                std::unique_ptr<MemoryBackend::DB>& m_dbRef;
                NotificationsAccumulator& m_notifications;
        };
    }

    struct MemoryBackend::Impl
    {
        NotificationsAccumulator m_notifications;
        TransactionManager<Transaction> m_tr;
    };


    MemoryBackend::MemoryBackend()
        : m_db(std::make_unique<DB>())
        , m_impl(std::make_unique<Impl>())
    {
        connect(&m_impl->m_notifications, &NotificationsAccumulator::photosAddedSignal,
                this, &MemoryBackend::photosAdded);
        connect(&m_impl->m_notifications, &NotificationsAccumulator::photosModifiedSignal,
                this, &MemoryBackend::photosModified);
    }


    MemoryBackend::~MemoryBackend()
    {

    }


    bool MemoryBackend::addPhotos(std::vector<Photo::DataDelta>& photos)
    {
        auto tr = openTransaction();

        std::vector<Photo::Id> ids;
        ids.reserve(photos.size());

        for (Photo::DataDelta& delta: photos)
        {
            assert(delta.getId().valid() == false);

            const Photo::Id id(m_db->m_nextPhotoId);
            delta.setId(id);

            auto [it, i] = m_db->m_photos.insert(StoregeDelta(delta));
            assert(i == true);

            if (delta.has(Photo::Field::People))
            {
                const auto& people = delta.get<Photo::Field::People>();
                auto& accessor = peopleInformationAccessor();

                for(const auto& person: people)
                {
                    const auto p_id = accessor.store(person.name);
                    const auto f_id = accessor.store(person.fingerprint);

                    const PersonInfo pf(p_id, delta.getId(), f_id, person.position);
                    accessor.store(pf);
                }
            }

            ids.push_back(id);

            m_db->m_nextPhotoId++;
        }

        m_impl->m_notifications.photosAdded(ids);

        return true;
    }


    bool MemoryBackend::update(const std::vector<Photo::DataDelta>& deltas)
    {
        auto tr = openTransaction();
        std::set<Photo::Id> ids;

        for (const auto& delta: deltas)
        {
            auto currentDelta = IBackend::getPhotoDelta<
                Photo::Field::Tags,
                Photo::Field::Flags,
                Photo::Field::Path,
                Photo::Field::Geometry,
                Photo::Field::GroupInfo,
                Photo::Field::PHash,
                Photo::Field::People
            >(delta.getId());

            photoChangeLogOperator().storeDifference(currentDelta, delta);

            currentDelta |= delta;

            auto it = m_db->m_photos.find(delta.getId());
            it = m_db->m_photos.erase(it);
            m_db->m_photos.insert(it, currentDelta);

            ids.insert(delta.getId());
        }

        m_impl->m_notifications.photosModified(ids);

        return true;
    }


    std::vector<TagValue> MemoryBackend::listTagValues(const Tag::Types& type, const Filter &)
    {
        std::set<TagValue> values;

        for(const auto& photo: m_db->m_photos)
        {
            const auto& tags = photo.get<Photo::Field::Tags>();
            auto it = tags.find(type);

            if (it != tags.end() && it->second.type() != Tag::ValueType::Empty)
                values.insert(it->second);
        }

        std::vector<TagValue> values_vec;
        values_vec.reserve(values.size());

        std::copy(values.begin(), values.end(), std::back_inserter(values_vec));

        return values_vec;
    }


    Photo::Data MemoryBackend::getPhoto(const Photo::Id& id)
    {
        Photo::Data data;
        auto it = m_db->m_photos.find(id);

        if (it != m_db->m_photos.end())
            data.apply(*it);

        return data;
    }


    Photo::DataDelta MemoryBackend::getPhotoDelta(const Photo::Id& id, const std::set<Photo::Field>& _fields)
    {
        const Photo::Data data = getPhoto(id);

        std::set<Photo::Field> fields = _fields;
        if (fields.empty())
        {
            const auto allEntries = magic_enum::enum_values<Photo::Field>();
            fields.insert(allEntries.begin(), allEntries.end());
        }

        Photo::DataDelta delta(id);

        if (fields.contains(Photo::Field::Path))
            delta.insert<Photo::Field::Path>(data.path);

        if (fields.contains(Photo::Field::Tags))
            delta.insert<Photo::Field::Tags>(data.tags);

        if (fields.contains(Photo::Field::Geometry))
        {
            const auto& geometry = data.geometry;

            if (geometry.isValid())
                delta.insert<Photo::Field::Geometry>(geometry);
        }

        if (fields.contains(Photo::Field::GroupInfo))
            delta.insert<Photo::Field::GroupInfo>(data.groupInfo);

        if (fields.contains(Photo::Field::Flags))
            delta.insert<Photo::Field::Flags>(data.flags);

        if (fields.contains(Photo::Field::PHash))
            delta.insert<Photo::Field::PHash>(data.phash);

        if (fields.contains(Photo::Field::People))
        {
            auto& peopleAccessor = peopleInformationAccessor();
            const auto peopleData = peopleAccessor.listPeopleFull(data.id);
            delta.insert<Photo::Field::People>(peopleData);
        }

        return delta;
    }


    int MemoryBackend::getPhotosCount(const Filter &)
    {
        return 0;
    }


    void MemoryBackend::set(const Photo::Id &id, const QString& name, int value)
    {
        auto tr = openTransaction();
        m_db->m_flags[id][name] = value;
        m_impl->m_notifications.photosModified({id});
    }


    std::optional<int> MemoryBackend::get(const Photo::Id& id, const QString& name)
    {
        std::optional<int> result;

        auto it = m_db->m_flags.find(id);

        if (it != m_db->m_flags.end())
        {
            auto f_it = it->second.find(name);

            if (f_it != it->second.end())
                result = f_it->second;
        }

        return result;
    }


    void MemoryBackend::setBits(const Photo::Id& id, const QString& name, int bits)
    {
        auto valueOpt = get(id, name);
        auto value = valueOpt.has_value()? *valueOpt : 0;

        value |= bits;
        set(id, name, value);
    }


    void MemoryBackend::clearBits(const Photo::Id& id, const QString& name, int bits)
    {
        auto valueOpt = get(id, name);
        auto value = valueOpt.has_value()? *valueOpt : 0;

        value &= ~bits;
        set(id, name, value);
    }


    void MemoryBackend::writeBlob(const Photo::Id& id, const QString& bt, const QByteArray& blob)
    {
        m_db->m_blobs[{id, bt}] = blob;
    }


    QByteArray MemoryBackend::readBlob(const Photo::Id& id, const QString& bt)
    {
        return m_db->m_blobs[{id, bt}];
    }


    std::vector<Photo::Id> MemoryBackend::markStagedAsReviewed()
    {
        std::vector<Photo::Id> ids;

        return ids;
    }


    BackendStatus MemoryBackend::init(const ProjectInfo& prjInfo)
    {
        BackendStatus status;

        if (prjInfo.backendName != "Memory")
            status = StatusCodes::OpenFailed;

        return status;
    }


    void MemoryBackend::closeConnections()
    {

    }

    std::shared_ptr<ITransaction> MemoryBackend::openTransaction()
    {
        return m_impl->m_tr.openTransaction(m_db, m_impl->m_notifications);
    }

    IGroupOperator& MemoryBackend::groupOperator()
    {
        return *this;
    }


    IPhotoOperator& MemoryBackend::photoOperator()
    {
        return *this;
    }


    IPhotoChangeLogOperator& MemoryBackend::photoChangeLogOperator()
    {
        return *this;
    }


    IPeopleInformationAccessor& MemoryBackend::peopleInformationAccessor()
    {
        return *this;
    }


    std::vector<PersonName> MemoryBackend::listPeople()
    {
        std::vector<PersonName> result;
        result.reserve(m_db->m_peopleNames.size());

        std::copy(m_db->m_peopleNames.cbegin(), m_db->m_peopleNames.cend(), std::back_inserter(result));

        return result;
    }


    std::vector<PersonInfo> MemoryBackend::listPeople(const Photo::Id& id)
    {
        std::vector<PersonInfo> people;

        std::copy_if(m_db->m_peopleInfo.cbegin(), m_db->m_peopleInfo.cend(), std::back_inserter(people), [id](const auto& info)
        {
            return info.ph_id == id;
        });

        return people;
    }


    PersonName MemoryBackend::person(const Person::Id& id)
    {
        auto it = m_db->m_peopleNames.find(id);
        return it == m_db->m_peopleNames.end()? PersonName(): *it;
    }


    std::vector<PersonFingerprint> MemoryBackend::fingerprintsFor(const Person::Id &)
    {
        std::vector<PersonFingerprint> fingerprints;

        return fingerprints;
    }


    std::map<PersonInfo::Id, PersonFingerprint> MemoryBackend::fingerprintsFor(const std::vector<PersonInfo::Id> &)
    {
        std::map<PersonInfo::Id, PersonFingerprint> fingerprints;

        return fingerprints;
    }


    Person::Id MemoryBackend::store(const PersonName &pn)
    {
        Person::Id id = pn.id();

        if (id.valid())
        {
            auto it = m_db->m_peopleNames.find(id);
            if (it == m_db->m_peopleNames.end())
                id = Person::Id();
            else
            {
                it = m_db->m_peopleNames.erase(it);
                m_db->m_peopleNames.insert(it, pn);
            }
        }
        else
        {
            auto it = std::find_if(m_db->m_peopleNames.cbegin(), m_db->m_peopleNames.cend(), [name = pn.name()](const auto& n)
            {
                return n.name() == name;
            });

            if (it == m_db->m_peopleNames.cend())
            {
                id = Person::Id(m_db->m_nextPersonName++);
                const PersonName personWithId(id, pn.name());

                m_db->m_peopleNames.insert(personWithId);
            }
            else
                id = it->id();
        }

        return id;
    }


    PersonFingerprint::Id MemoryBackend::store(const PersonFingerprint &)
    {
        PersonFingerprint::Id id;

        return id;
    }


    void MemoryBackend::dropPersonInfo(const PersonInfo::Id& id)
    {
        auto it = m_db->m_peopleInfo.find(id);
        if (it != m_db->m_peopleInfo.end())
            m_db->m_peopleInfo.erase(it);
    }


    PersonInfo::Id MemoryBackend::storePerson(const PersonInfo& pi)
    {
        auto mpi = pi;
        if (mpi.id.valid() == false)
            mpi.id = m_db->m_nextPersonInfo++;

        auto it = m_db->m_peopleInfo.find(mpi.id);
        if (it == m_db->m_peopleInfo.end())
            m_db->m_peopleInfo.insert(mpi);
        else
        {
            it = m_db->m_peopleInfo.erase(it);
            m_db->m_peopleInfo.insert(it, mpi);
        }

        return mpi.id;
    }


    void MemoryBackend::append(const Photo::Id& id, Operation operation, Field field, const QString& data)
    {
        const auto entry = std::make_tuple(id, operation, field, data);
        m_db->m_logEntries.push_back(entry);
    }


    QStringList MemoryBackend::dumpChangeLog()
    {
        QStringList list;

        for(const auto& entry: m_db->m_logEntries)
        {
            const QString formatted = format(std::get<0>(entry).value(),
                                             std::get<1>(entry),
                                             std::get<2>(entry),
                                             std::get<3>(entry)
            );

            list.append(formatted);
        }

        return list;
    }


    Group::Id MemoryBackend::addGroup(const Photo::Id& representative_photo, Group::Type type)
    {
        Group::Id gid(m_db->m_nextGroup++);
        m_db->m_groups.emplace(gid, std::pair(representative_photo, type));

        GroupInfo group(gid, GroupInfo::Role::Representative);
        Photo::DataDelta delta(representative_photo);
        delta.insert<Photo::Field::GroupInfo>(group);

        update( {delta} );

        return gid;
    }


    Photo::Id MemoryBackend::removeGroup(const Group::Id& gid)
    {
        Photo::Id r_id;

        auto it = m_db->m_groups.find(gid);

        if (it != m_db->m_groups.end())
        {
            m_db->m_groups.erase(gid);

            std::vector<Photo::Id> photosToClear;

            for (const StoregeDelta& delta: m_db->m_photos)
            {
                const auto& groupInfo = delta.get<Photo::Field::GroupInfo>();

                if (groupInfo.role != GroupInfo::Role::None &&
                    groupInfo.group_id == gid)
                {
                    const Photo::Id id = delta.getId();

                    photosToClear.push_back(id);

                    if (groupInfo.role == GroupInfo::Role::Representative)
                        r_id = id;
                }
            }

            std::vector<Photo::DataDelta> deltas;
            deltas.reserve(photosToClear.size());

            for (const Photo::Id& id: photosToClear)
            {
                Photo::DataDelta delta(id);
                GroupInfo info;
                delta.insert<Photo::Field::GroupInfo>(info);
                deltas.push_back(delta);
            }

            update(deltas);
        }

        return r_id;
    }


    Group::Type MemoryBackend::type(const Group::Id &) const
    {
        Group::Type t = Group::Type::Invalid;

        return t;
    }


    std::vector<Photo::Id> MemoryBackend::membersOf(const Group::Id& id) const
    {
        std::vector<StoregeDelta> members;
        std::copy_if(m_db->m_photos.begin(), m_db->m_photos.end(), std::back_inserter(members), [id](const StoregeDelta& data)
        {
            const auto& groupInfo = data.get<Photo::Field::GroupInfo>();
            return groupInfo.group_id == id && groupInfo.role == GroupInfo::Role::Member;
        });

        std::vector<Photo::Id> ids;
        std::transform(members.begin(), members.end(), std::back_inserter(ids), [](const StoregeDelta& d){ return d.getId(); });

        return ids;
    }


    std::vector<Group::Id> MemoryBackend::listGroups() const
    {
        std::vector<StoregeDelta> representatives;
        std::copy_if(m_db->m_photos.begin(), m_db->m_photos.end(), std::back_inserter(representatives), [](const StoregeDelta& data)
        {
            return data.get<Photo::Field::GroupInfo>().role == GroupInfo::Role::Representative;
        });

        std::vector<GroupInfo> infos;
        std::transform(representatives.begin(), representatives.end(), std::back_inserter(infos), [](const StoregeDelta& d){ return d.get<Photo::Field::GroupInfo>(); });

        std::vector<Group::Id> ids;
        std::transform(infos.begin(), infos.end(), std::back_inserter(ids), &extract<GroupInfo, Group::Id, &GroupInfo::group_id>);

        return ids;
    }


    bool MemoryBackend::removePhoto(const Photo::Id &)
    {
        return false;
    }


    bool MemoryBackend::removePhotos(const Filter &)
    {
        return false;
    }


    std::vector<Photo::Id> MemoryBackend::onPhotos(const Filter& filters, const Action& action)
    {
        std::vector<Photo::Id> ids = getPhotos(filters);

        std::vector<Photo::Data> photo_data;
        for(const auto id: ids)
            photo_data.push_back(getPhoto(id));

        onPhotos(photo_data, action);

        std::transform(photo_data.cbegin(), photo_data.cend(), ids.begin(), [](const Photo::Data& data)
        {
            return data.id;
        });

        return ids;
    }


    std::vector<Photo::Id> MemoryBackend::getPhotos(const Filter& filter)
    {
        std::vector<StoregeDelta> data(m_db->m_photos.begin(), m_db->m_photos.end());
        data = filterPhotos(data, *m_db, filter);

        std::vector<Photo::Id> ids;
        for(const auto& photo: data)
            ids.push_back(photo.getId());

        return ids;
    }


    void MemoryBackend::setPHash(const Photo::Id& id, const Photo::PHashT& phash)
    {
        auto it = m_db->m_photos.find(id);

        if (it != m_db->m_photos.end())
        {
            auto data(*it);
            data.insert<Photo::Field::PHash>(phash);
            m_db->m_photos.erase(it);
            m_db->m_photos.insert(data);
        }
    }


    std::optional<Photo::PHashT> MemoryBackend::getPHash(const Photo::Id& id)
    {
        auto it = m_db->m_photos.find(id);

        std::optional<Photo::PHashT> result;

        if (it != m_db->m_photos.end() && it->get<Photo::Field::PHash>().valid())
            result = it->get<Photo::Field::PHash>();

        return result;
    }


    bool MemoryBackend::hasPHash(const Photo::Id& id)
    {
        auto it = m_db->m_photos.find(id);

        return it == m_db->m_photos.end()? false: it->get<Photo::Field::PHash>().valid();
    }


    Photo::Id MemoryBackend::getIdFor(const StoregeDelta& d)
    {
        return d.getId();
    }


    Person::Id MemoryBackend::getIdFor(const PersonName& pn)
    {
        return pn.id();
    }


    PersonInfo::Id MemoryBackend::getIdFor(const PersonInfo& pi)
    {
        return pi.id;
    }


    void MemoryBackend::onPhotos(std::vector<Photo::Data>& photo_data, const Action& action) const
    {
        if (auto sort_action = std::get_if<Actions::SortByTag>(&action))
        {
            std::stable_sort(photo_data.begin(), photo_data.end(), [sort_action](const auto& lhs, const auto& rhs)
            {
                return tristate_compare(lhs, rhs, sort_action->tag, sort_action->sort_order) < 0;
            });
        }
        else if (auto group_action = std::get_if<Actions::GroupAction>(&action))
        {
            // sort by actions in reverse order so first one has greates influence.
            for (auto it = group_action->actions.rbegin(); it != group_action->actions.rend(); ++it)
                onPhotos(photo_data, *it);
        }
        else if (auto sort = std::get_if<Actions::Sort>(&action))
        {
            switch(sort->by)
            {
                case Actions::Sort::By::PHash:
                    if (sort->order == Qt::AscendingOrder)
                        std::stable_sort(photo_data.begin(), photo_data.end(), &PhotoData::isLess<Photo::Field::PHash>);
                    else
                        std::stable_sort(photo_data.rbegin(), photo_data.rend(), &PhotoData::isLess<Photo::Field::PHash>);
                    break;

                case Actions::Sort::By::Timestamp:
                {
                    const Actions::SortByTag byDate(Tag::Types::Date, sort->order);
                    const Actions::SortByTag byTime(Tag::Types::Time, sort->order);
                    onPhotos(photo_data, byTime);
                    onPhotos(photo_data, byDate);

                    break;
                }

                case Actions::Sort::By::ID:
                {
                    if (sort->order == Qt::AscendingOrder)
                        std::sort(photo_data.begin(), photo_data.end(), [](const auto& lhs, const auto& rhs)
                        {
                            return lhs.id < rhs.id;
                        });
                    else
                        std::sort(photo_data.begin(), photo_data.end(), [](const auto& lhs, const auto& rhs)
                        {
                            return lhs.id > rhs.id;
                        });

                    break;
                }
            }
        }
        else
            assert(!"Unknown action");
    }

}
