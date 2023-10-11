
#include <core/task_executor_utils.hpp>
#include <database/database_executor_traits.hpp>

#include "faces_saver.hpp"


FacesSaver::FacesSaver(Database::IDatabase& db)
    : m_db(db)
{
    m_people = fetchPeople();
}


FacesSaver::~FacesSaver()
{

}


void FacesSaver::store(FaceInfo& face)
{
    store_person_name(face);
    store_fingerprint(face);

    // update names assigned to face
    face.face.p_id = face.person.id();

    // update fingerprints assigned to face
    if (face.face.f_id.valid() == false)
        face.face.f_id = face.fingerprint.id();

    store_person_information(face);
}


void FacesSaver::store_person_name(FaceInfo& face)
{
    const bool nameChanged =
        face.person.id().valid() == false && face.person.name().isEmpty() == false;

    if (nameChanged)
    {
        // introduce name associated with face to db (if needed)
        const QString& name = face.person.name();

        auto it = std::find_if(m_people.cbegin(), m_people.cend(), [name](const PersonName& d)
        {
            return d.name() == name;
        });

        if (it == m_people.cend())        // new name, store it in db
        {
            face.person = storeNewPerson(name);
            m_people.push_back(face.person);
        }
        else
            face.person = *it;
    }
}


void FacesSaver::store_fingerprint(FaceInfo& face)
{
    if (face.fingerprint.id().valid() == false)
    {
        const PersonFingerprint::Id fid =
            evaluate<PersonFingerprint::Id(Database::IBackend &)>(m_db, [fingerprint = face.fingerprint](Database::IBackend& backend)
        {
            return backend.peopleInformationAccessor().store(fingerprint);
        });

        const PersonFingerprint fingerprint(fid, face.fingerprint.fingerprint());
        face.fingerprint = fingerprint;
    }
}


void FacesSaver::store_person_information(const FaceInfo& face)
{
    const PersonInfo& faceInfo = face.face;
    const PersonFingerprint& fingerprint = face.fingerprint;

    m_db.exec([faceInfo, fingerprint](Database::IBackend& backend)
    {
        backend.peopleInformationAccessor().store(faceInfo);
    });
}


std::vector<PersonName> FacesSaver::fetchPeople() const
{
    return evaluate<std::vector<PersonName>(Database::IBackend &)>(m_db, [](Database::IBackend& backend)
    {
        auto people = backend.peopleInformationAccessor().listPeople();

        return people;
    });
}


PersonName FacesSaver::storeNewPerson(const QString& name) const
{
    const PersonName person = evaluate<PersonName (Database::IBackend &)>
            (m_db, [name](Database::IBackend& backend)
    {
        const PersonName d(Person::Id(), name);
        const auto id = backend.peopleInformationAccessor().store(d);
        return PersonName(id, name);
    });

    return person;
}
