
#include <QFileInfo>

#include "memory_backend.hpp"
#include "database/project_info.hpp"


namespace
{
    template<typename T>
    bool compare(const T& lhs, const T& rhs, Qt::SortOrder order)
    {
        return order == Qt::AscendingOrder? lhs < rhs: rhs < lhs;
    }

    int tristate_compare(const Photo::Data& lhs, const Photo::Data& rhs, const TagTypes& tagType, Qt::SortOrder order)
    {
        const auto l_it = lhs.tags.find(tagType);
        const auto r_it = rhs.tags.find(tagType);
        const auto l_tag = l_it == lhs.tags.end()? TagValue(): l_it->second;
        const auto r_tag = r_it == rhs.tags.end()? TagValue(): r_it->second;

        const bool is_less = compare(l_tag, r_tag, order);
        const bool is_greater = compare(r_tag, l_tag, order);

        return (is_less? -1: 0) + (is_greater? 1: 0);
    }
}


namespace Database
{

    bool MemoryBackend::addPhotos(std::vector<Photo::DataDelta>& photos)
    {
        std::vector<Photo::Id> ids;
        ids.reserve(photos.size());

        for (Photo::DataDelta& delta: photos)
        {
            assert(delta.getId().valid() == false);

            const Photo::Id id(m_nextPhotoId);
            delta.setId(id);

            Photo::Data data;
            data.apply(delta);

            auto [it, i] = m_photos.insert(data);      // insert empty Data for given id
            assert(i == true);

            ids.push_back(data.id);

            m_nextPhotoId++;
        }

        emit photosAdded(ids);

        return true;
    }


    bool MemoryBackend::update(const std::vector<Photo::DataDelta>& deltas)
    {
        std::set<Photo::Id> ids;

        for (const auto& delta: deltas)
        {
            auto it = m_photos.find(delta.getId());

            Photo::Data data = *it;
            photoChangeLogOperator().storeDifference(data, delta);

            data.apply(delta);

            it = m_photos.erase(it);
            m_photos.insert(it, data);

            ids.insert(delta.getId());
        }

        emit photosModified(ids);

        return true;
    }


    std::vector<TagValue> MemoryBackend::listTagValues(const TagTypes& type, const Filter &)
    {
        std::set<TagValue> values;

        for(const auto& photo: m_photos)
        {
            auto it = photo.tags.find(type);
            if (it != photo.tags.end() && it->second.type() != Tag::ValueType::Empty)
                values.insert(it->second);
        }

        std::vector<TagValue> values_vec;
        values_vec.reserve(values.size());

        std::copy(values.begin(), values.end(), std::back_inserter(values_vec));

        return values_vec;
    }


    Photo::Data MemoryBackend::getPhoto(const Photo::Id& id)
    {
        auto it = m_photos.find(id);
        return it == m_photos.end()? Photo::Data(): *it;
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

        if (fields.contains(Photo::Field::Checksum))
        {
            const auto& checksum = data.sha256Sum;

            if (checksum.isEmpty() == false)
                delta.insert<Photo::Field::Checksum>(checksum);
        }

        if (fields.contains(Photo::Field::GroupInfo))
            delta.insert<Photo::Field::GroupInfo>(data.groupInfo);

        if (fields.contains(Photo::Field::Flags))
            delta.insert<Photo::Field::Flags>(data.flags);

        return delta;
    }


    int MemoryBackend::getPhotosCount(const Filter &)
    {
        return 0;
    }


    void MemoryBackend::set(const Photo::Id &id, const QString& name, int value)
    {
        m_flags[id][name] = value;
    }


    std::optional<int> MemoryBackend::get(const Photo::Id& id, const QString& name)
    {
        std::optional<int> result;

        auto it = m_flags.find(id);

        if (it != m_flags.end())
        {
            auto f_it = it->second.find(name);

            if (f_it != it->second.end())
                result = f_it->second;
        }

        return result;
    }


    void MemoryBackend::setThumbnail(const Photo::Id& id, const QByteArray& thumbnail)
    {
        m_thumbnails[id] = thumbnail;
    }


    QByteArray MemoryBackend::getThumbnail(const Photo::Id& id)
    {
        return m_thumbnails[id];
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
        result.reserve(m_peopleNames.size());

        std::copy(m_peopleNames.cbegin(), m_peopleNames.cend(), std::back_inserter(result));

        return result;
    }


    std::vector<PersonInfo> MemoryBackend::listPeople(const Photo::Id& id)
    {
        std::vector<PersonInfo> people;

        std::copy_if(m_peopleInfo.cbegin(), m_peopleInfo.cend(), std::back_inserter(people), [id](const auto& info)
        {
            return info.ph_id == id;
        });

        return people;
    }


    PersonName MemoryBackend::person(const Person::Id& id)
    {
        auto it = m_peopleNames.find(id);
        return it == m_peopleNames.end()? PersonName(): *it;
    }


    std::vector<PersonFingerprint> MemoryBackend::fingerprintsFor(const Person::Id &)
    {
        std::vector<PersonFingerprint> fingerprints;

        return fingerprints;
    }


    std::map<PersonInfo::Id, PersonFingerprint> MemoryBackend::fingerprintsFor(const std::vector<PersonInfo::Id>& id)
    {
        std::map<PersonInfo::Id, PersonFingerprint> fingerprints;

        return fingerprints;
    }


    Person::Id MemoryBackend::store(const PersonName &pn)
    {
        Person::Id id = pn.id();

        if (id.valid())
        {
            auto it = m_peopleNames.find(id);
            if (it == m_peopleNames.end())
                id = Person::Id();
            else
            {
                it = m_peopleNames.erase(it);
                m_peopleNames.insert(it, pn);
            }
        }
        else
        {
            auto it = std::find_if(m_peopleNames.cbegin(), m_peopleNames.cend(), [name = pn.name()](const auto& n)
            {
                return n.name() == name;
            });

            if (it == m_peopleNames.cend())
            {
                id = Person::Id(m_nextPersonName++);
                const PersonName personWithId(id, pn.name());

                m_peopleNames.insert(personWithId);
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
        auto it = m_peopleInfo.find(id);
        if (it != m_peopleInfo.end())
            m_peopleInfo.erase(it);
    }


    PersonInfo::Id MemoryBackend::storePerson(const PersonInfo& pi)
    {
        auto mpi = pi;
        if (mpi.id.valid() == false)
            mpi.id = m_nextPersonInfo++;

        auto it = m_peopleInfo.find(mpi.id);
        if (it == m_peopleInfo.end())
            m_peopleInfo.insert(mpi);
        else
        {
            it = m_peopleInfo.erase(it);
            m_peopleInfo.insert(it, mpi);
        }

        return mpi.id;
    }


    void MemoryBackend::append(const Photo::Id& id, Operation operation, Field field, const QString& data)
    {
        const auto entry = std::make_tuple(id, operation, field, data);
        m_logEntries.push_back(entry);
    }


    QStringList MemoryBackend::dumpChangeLog()
    {
        QStringList list;

        for(const auto& entry: m_logEntries)
        {
            const QString formatted = format(std::get<0>(entry),
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
        Group::Id gid(m_nextGroup++);
        m_groups.emplace(gid, std::pair(representative_photo, type));

        GroupInfo group(gid, GroupInfo::Role::Representative);
        Photo::DataDelta delta(representative_photo);
        delta.insert<Photo::Field::GroupInfo>(group);

        update( {delta} );

        return gid;
    }


    Photo::Id MemoryBackend::removeGroup(const Group::Id& gid)
    {
        Photo::Id r_id;

        auto it = m_groups.find(gid);

        if (it != m_groups.end())
        {
            m_groups.erase(gid);

            std::vector<Photo::Id> photosToClear;

            for(Photo::Data data: m_photos)
                if (data.groupInfo.role != GroupInfo::Role::None &&
                    data.groupInfo.group_id == gid)
                {
                    const Photo::Id id = data.id;

                    photosToClear.push_back(id);

                    if (data.groupInfo.role == GroupInfo::Role::Representative)
                        r_id = id;
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


    std::vector<Photo::Id> MemoryBackend::membersOf(const Group::Id &) const
    {
        std::vector<Photo::Id> ids;

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


    std::vector<Photo::Id> MemoryBackend::getPhotos(const Filter &)
    {
        std::vector<Photo::Id> ids;

        for(const auto& photo: m_photos)
            ids.push_back(photo.id);

        return ids;
    }


    Photo::Id MemoryBackend::getIdFor(const Photo::Data& d)
    {
        return d.id;
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
        else if(auto sort_action = std::get_if<Actions::SortByTimestamp>(&action))
        {
            const Actions::SortByTag byDate(TagTypes::Date, sort_action->sort_order);
            const Actions::SortByTag byTime(TagTypes::Time, sort_action->sort_order);
            onPhotos(photo_data, byTime);
            onPhotos(photo_data, byDate);
        }
        else if (std::get_if<Actions::SortByID>(&action))
        {
            std::sort(photo_data.begin(), photo_data.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.id < rhs.id;
            });
        }
        else if (auto group_action = std::get_if<Actions::GroupAction>(&action))
        {
            // sort by actions in reverse order so first one has greates influence.
            for (auto it = group_action->actions.rbegin(); it != group_action->actions.rend(); ++it)
                onPhotos(photo_data, *it);
        }
        else
        {
            assert(!"Unknown action");
        }
    }

}
