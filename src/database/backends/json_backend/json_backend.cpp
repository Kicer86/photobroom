
#include <QFileInfo>

#include "json_backend.hpp"
#include "database/project_info.hpp"


namespace Database
{

    bool JsonBackend::addPhotos(std::vector<Photo::DataDelta>& photos)
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


    bool JsonBackend::update(const Photo::DataDelta& delta)
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


    std::vector<TagTypeInfo> JsonBackend::listTags()
    {

    }


    std::vector<TagValue> JsonBackend::listTagValues(const TagTypes &, const std::vector<IFilter::Ptr> &)
    {

    }


    Photo::Data JsonBackend::getPhoto(const Photo::Id& id)
    {
        auto it = m_photos.find(id);
        return it == m_photos.end()? Photo::Data(): *it;
    }


    int JsonBackend::getPhotosCount(const std::vector<IFilter::Ptr> &)
    {

    }


    void JsonBackend::set(const Photo::Id &id, const QString& name, int value)
    {
        m_flags[id][name] = value;
    }


    std::optional<int> JsonBackend::get(const Photo::Id& id, const QString& name)
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


    std::vector<Photo::Id> JsonBackend::markStagedAsReviewed()
    {

    }


    BackendStatus JsonBackend::init(const ProjectInfo& prjInfo)
    {
        BackendStatus status;

        const QFileInfo db_file_info(prjInfo.databaseLocation);

        if (prjInfo.backendName != "Json")
            status = StatusCodes::OpenFailed;
        else if (db_file_info.exists())
        {
            if (db_file_info.isReadable()  &&
                db_file_info.isWritable())
            {

            }
            else
                status = StatusCodes::OpenFailed;
        }
        else
        {
            QFile db_file(prjInfo.databaseLocation);
            const bool open_status = db_file.open(QIODevice::NewOnly | QIODevice::ReadWrite);

            if (open_status == false)
                status = StatusCodes::OpenFailed;
        }

        return status;
    }


    void JsonBackend::closeConnections()
    {

    }


    IGroupOperator& JsonBackend::groupOperator()
    {
        return *this;
    }


    IPhotoOperator& JsonBackend::photoOperator()
    {

    }


    IPhotoChangeLogOperator& JsonBackend::photoChangeLogOperator()
    {
        return *this;
    }


    IPeopleInformationAccessor& JsonBackend::peopleInformationAccessor()
    {
        return *this;
    }


    std::vector<PersonName> JsonBackend::listPeople()
    {
        std::vector<PersonName> result;
        result.reserve(m_peopleNames.size());

        std::copy(m_peopleNames.cbegin(), m_peopleNames.cend(), std::back_inserter(result));

        return result;
    }


    std::vector<PersonInfo> JsonBackend::listPeople(const Photo::Id& id)
    {
        std::vector<PersonInfo> people;

        std::copy_if(m_peopleInfo.cbegin(), m_peopleInfo.cend(), std::back_inserter(people), [id](const auto& info)
        {
            return info.ph_id == id;
        });

        return people;
    }


    PersonName JsonBackend::person(const Person::Id& id)
    {
        auto it = m_peopleNames.find(id);
        return it == m_peopleNames.end()? PersonName(): *it;


    }


    std::vector<PersonFingerprint> JsonBackend::fingerprintsFor(const Person::Id &)
    {

    }


    std::map<PersonInfo::Id, PersonFingerprint> JsonBackend::fingerprintsFor(const std::vector<PersonInfo::Id>& id)
    {

    }


    Person::Id JsonBackend::store(const PersonName &pn)
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


    PersonFingerprint::Id JsonBackend::store(const PersonFingerprint &)
    {

    }


    void JsonBackend::dropPersonInfo(const PersonInfo::Id& id)
    {
        auto it = m_peopleInfo.find(id);
        if (it != m_peopleInfo.end())
            m_peopleInfo.erase(it);
    }


    PersonInfo::Id JsonBackend::storePerson(const PersonInfo& pi)
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


    void JsonBackend::append(const Photo::Id& id, Operation operation, Field field, const QString& data)
    {
        const auto entry = std::make_tuple(id, operation, field, data);
        m_logEntries.push_back(entry);
    }


    QStringList JsonBackend::dumpChangeLog()
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


    Group::Id JsonBackend::addGroup(const Photo::Id& representative_photo, Group::Type type)
    {
        Group::Id gid(m_nextGroup++);
        m_groups.emplace(gid, std::pair(representative_photo, type));

        GroupInfo group(gid, GroupInfo::Role::Representative);
        Photo::DataDelta delta(representative_photo);
        delta.insert<Photo::Field::GroupInfo>(group);

        update(delta);

        return gid;
    }


    Photo::Id JsonBackend::removeGroup(const Group::Id& gid)
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


    Group::Type JsonBackend::type(const Group::Id &) const
    {

    }


    std::vector<Photo::Id> JsonBackend::membersOf(const Group::Id &) const
    {

    }


    Photo::Id JsonBackend::getIdFor(const Photo::Data& d)
    {
        return d.id;
    }


    Person::Id JsonBackend::getIdFor(const PersonName& pn)
    {
        return pn.id();
    }


    PersonInfo::Id JsonBackend::getIdFor(const PersonInfo& pi)
    {
        return pi.id;
    }
}
