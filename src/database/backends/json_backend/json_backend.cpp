
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

            const Photo::Id id(m_nextId);
            delta.setId(id);

            Photo::Data data;
            data.apply(delta);

            auto [it, i] = m_photos.insert(data);      // insert empty Data for given id

            assert(i == true);

            m_nextId++;
        }

        return true;
    }


    bool JsonBackend::update(const Photo::DataDelta& delta)
    {
        auto it = m_photos.find(delta.getId());

        Photo::Data data = *it;
        data.apply(delta);

        it = m_photos.erase(it);
        m_photos.insert(it, data);

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
        auto it = m_people.find(id);
        return it == m_people.end()? std::vector<PersonInfo>(): it->second;
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
            id = Person::Id(m_nextPerson++);
            const PersonName personWithId(id, pn.name());

            m_peopleNames.insert(personWithId);
        }

        return id;
    }


    PersonInfo::Id JsonBackend::store(const PersonInfo& pi)
    {

    }


    PersonFingerprint::Id JsonBackend::store(const PersonFingerprint &)
    {

    }


    void JsonBackend::storeDifference(const Photo::Data &, const Photo::DataDelta &)
    {

    }


    void JsonBackend::groupCreated(const Group::Id &, const Group::Type &, const Photo::Id& representative)
    {

    }


    void JsonBackend::groupDeleted(const Group::Id &, const Photo::Id& representative, const std::vector<Photo::Id>& members)
    {

    }


    QStringList JsonBackend::dumpChangeLog()
    {
        return {};
    }
}
