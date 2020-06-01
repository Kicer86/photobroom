
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
}


namespace Database
{

    bool MemoryBackend::addPhotos(std::vector<Photo::DataDelta>& photos)
    {
        for (Photo::DataDelta& delta: photos)
        {
            assert(delta.getId().valid() == false);

            const Photo::Id id(m_nextPhotoId);
            delta.setId(id);

            Photo::Data data;
            data.apply(delta);

            auto [it, i] = m_photos.insert(data);      // insert empty Data for given id

            assert(i == true);

            m_nextPhotoId++;
        }

        return true;
    }


    bool MemoryBackend::update(const Photo::DataDelta& delta)
    {
        auto it = m_photos.find(delta.getId());

        Photo::Data data = *it;
        photoChangeLogOperator().storeDifference(data, delta);

        data.apply(delta);

        it = m_photos.erase(it);
        m_photos.insert(it, data);

        emit photoModified(delta.getId());

        return true;
    }


    std::vector<TagValue> MemoryBackend::listTagValues(const TagTypes &, const std::vector<IFilter::Ptr> &)
    {
        std::vector<TagValue> values;

        return values;
    }


    Photo::Data MemoryBackend::getPhoto(const Photo::Id& id)
    {
        auto it = m_photos.find(id);
        return it == m_photos.end()? Photo::Data(): *it;
    }


    int MemoryBackend::getPhotosCount(const std::vector<IFilter::Ptr> &)
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

        update(delta);

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

            for (const Photo::Id& id: photosToClear)
            {
                Photo::DataDelta delta(id);
                GroupInfo info;
                delta.insert<Photo::Field::GroupInfo>(info);
                update(delta);
            }
        }

        return r_id;
    }


    Group::Type MemoryBackend::type(const Group::Id &) const
    {
        Group::Type t;

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


    bool MemoryBackend::removePhotos(const std::vector<IFilter::Ptr> &)
    {
        return false;
    }


    std::vector<Photo::Id> MemoryBackend::onPhotos(const std::vector<IFilter::Ptr>& filters, const Actions& action)
    {
        std::vector<Photo::Id> ids = getPhotos(filters);

        if (auto sort_action = std::get_if<SortAction>(&action))
        {
            std::vector<Photo::Data> photo_data;
            for(const auto id: ids)
                photo_data.push_back(getPhoto(id));

            std::sort(photo_data.begin(), photo_data.end(), [sort_action](const auto& lhs, const auto& rhs)
            {
                const auto l_it = lhs.tags.find(sort_action->tag);
                const auto r_it = rhs.tags.find(sort_action->tag);
                const auto l_tag = l_it == lhs.tags.end()? TagValue(): l_it->second;
                const auto r_tag = r_it == rhs.tags.end()? TagValue(): r_it->second;

                return compare(l_tag, r_tag, sort_action->sort_order);
            });

            std::transform(photo_data.cbegin(), photo_data.cend(), ids.begin(), [](const Photo::Data& data)
            {
                return data.id;
            });
        }
        else
        {
            assert(!"Unknown action");
        }

        return ids;
    }


    std::vector<Photo::Id> MemoryBackend::getPhotos(const std::vector<IFilter::Ptr> &)
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

}
