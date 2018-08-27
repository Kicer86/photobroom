/*
 * Access information about people from db and photo itself.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "people_operator.hpp"

#include <functional>

#include <QFileInfo>

#include <core/icore_factory_accessor.hpp>
#include <core/task_executor_utils.hpp>
#include <database/idatabase.hpp>
#include <face_recognition/face_recognition.hpp>

#include "people_operator_p.hpp"


// TODO: tasks are not using any shared data and therefore ask database for the same stuff over and over.


using namespace std::placeholders;

template<typename T>
struct ExecutorTraits<Database::IDatabase, T>
{
    static void exec(Database::IDatabase* db, T&& t)
    {
        db->performCustomAction(std::forward<T>(t));
    }
};

namespace
{
    const QString faces_recognized_flag = QStringLiteral("faces_recognized");
}


FaceTask::FaceTask(const Photo::Id& id, Database::IDatabase* d):
    m_id(id),
    m_db(d)
{

}


FaceTask::~FaceTask()
{

}


std::vector<QRect> FaceTask::fetchFacesFromDb() const
{
    return evaluate<std::vector<QRect>(Database::IBackendOperator*)>
        (m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        std::vector<QRect> faces;

        const auto people = backend->listPeople(id);
        for(const auto& person: people)
            if (person.rect.isValid())
                faces.push_back(person.rect);

        return faces;
    });
}


QString FaceTask::getPhotoPath() const
{
    return evaluate<QString(Database::IBackendOperator* backend)>(m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        auto photo = backend->getPhotoFor(id);

        return photo->getPath();
    });
}


///////////////////////////////////////////////////////////////////////////////


FacesFetcher::FacesFetcher(const Photo::Id& id, ICoreFactoryAccessor* c, Database::IDatabase* db):
    FaceTask(id, db),
    m_coreFactory(c)
{
    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");
}


FacesFetcher::~FacesFetcher()
{

}


std::string FacesFetcher::name() const
{
    return "FacesFetcher";
}


void FacesFetcher::perform()
{
    QVector<QRect> result;
    const std::vector<QRect> list_of_faces = fetchFacesFromDb();

    if (list_of_faces.empty())
    {
        const QString path = getPhotoPath();
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(m_coreFactory);
        const auto faces = face_recognition.fetchFaces(full_path);

        for(const QRect& face: faces)
            result.append(face);
    }
    else
    {
        result.reserve(static_cast<int>(list_of_faces.size()));

        std::copy(list_of_faces.cbegin(), list_of_faces.cend(), std::back_inserter(result));
    }

    emit faces(result);
}


///////////////////////////////////////////////////////////////////////////////


FaceRecognizer::FaceRecognizer(const PeopleOperator::FaceLocation& face,  const QString& patterns, ICoreFactoryAccessor* core, Database::IDatabase* db):
    FaceTask(face.first, db),
    m_data(face),
    m_patterns(patterns),
    m_coreFactory(core)
{

}


FaceRecognizer::~FaceRecognizer()
{

}


std::string FaceRecognizer::name() const
{
    return "FaceRecognizer";
}


void FaceRecognizer::perform()
{
    const std::vector<PersonInfo> peopleData = fetchPeopleFromDb();
    PersonName result;

    // check if we have data for given face in db
    bool filled = false;
    for(const PersonInfo& person: peopleData)
        if (person.rect.isValid() &&
            person.rect == m_data.second &&
            person.p_id.valid())
        {
            result = personData(person.p_id);         // use stored name
            filled = true;
            break;
        }

    if (filled == false && wasAnalyzed() == false)    // we do not have data, try to guess
    {
        const QString path = getPhotoPath();
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(m_coreFactory);
        const QString personPath = face_recognition.recognize(full_path, m_data.second, m_patterns);

        if (personPath.isEmpty() == false)
        {
            const QFileInfo pathInfo(personPath);
            const QString personId = pathInfo.baseName();
            const Person::Id pid(personId.toInt());
            result = personData(pid);
        }
    }

    emit recognized(m_data.second, result);
}


PersonName FaceRecognizer::personData(const Person::Id& id) const
{
    const PersonName person =
        evaluate<PersonName (Database::IBackendOperator *)>
            (m_db, [id](Database::IBackendOperator* backend)
    {
        const auto people = backend->person(id);

        return people;
    });

    return person;
}


std::vector<PersonInfo> FaceRecognizer::fetchPeopleFromDb() const
{
    return evaluate<std::vector<PersonInfo>(Database::IBackendOperator* backend)>(m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        auto people = backend->listPeople(id);

        return people;
    });
}


bool FaceRecognizer::wasAnalyzed() const
{
    return evaluate<bool(Database::IBackendOperator* backend)>(m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        const auto value = backend->get(id, faces_recognized_flag);
        const bool result = value.has_value() && *value > 0;

        return result;
    });
}


///////////////////////////////////////////////////////////////////////////////


FetchUnassigned::FetchUnassigned(const Photo::Id& id, Database::IDatabase* db):
    m_id(id),
    m_db(db)
{
}


FetchUnassigned::~FetchUnassigned()
{
}


std::string FetchUnassigned::name() const
{
    return "FetchUnassigned";
}


void FetchUnassigned::perform()
{
    const QStringList un = evaluate<QStringList(Database::IBackendOperator* backend)>(m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        const std::vector<PersonInfo> people = backend->listPeople(id);
        QStringList locatedPeople;

        for(const PersonInfo& person: people)
        {
            if (person.p_id.valid() && person.rect.isEmpty())
            {
                const PersonName data = backend->person(person.p_id);
                locatedPeople.append(data.name());
            }
        }

        return locatedPeople;
    });

    emit unassigned(m_id, un);
}


///////////////////////////////////////////////////////////////////////////////


FaceStore::FaceStore(const Photo::Id& id,
                     const std::vector<PeopleOperator::FaceInfo>& known_people,
                     const QStringList& unknown_people,
                     Database::IDatabase* db,
                     const QString& patterns):
    FaceTask(id, db),
    m_knownPeople(known_people),
    m_unknownPeople(unknown_people),
    m_patterns(patterns)
{

}


FaceStore::~FaceStore()
{

}


std::string FaceStore::name() const
{
    return "FaceStore";
}


void FaceStore::perform()
{
    const std::vector<PersonName> people = fetchPeople();
    const QString path = getPhotoPath();
    const QImage image(path);

    // store people data
    for (const auto& person: m_knownPeople )
    {
        const QString& name = person.second;
        const QRect& face_coords = person.first;

        auto it = std::find_if(people.cbegin(),
                               people.cend(),
                               [name](const PersonName& d)
        {
            return d.name() == name;
        });

        if (it == people.cend())  // we do not know that person
        {
            m_db->performCustomAction([base_path = m_patterns,
                                       face = image.copy(face_coords),
                                       ph_id = m_id,
                                       person]
                                      (Database::IBackendOperator* op)
            {
                const QRect& face_location = person.first;
                const QString& name = person.second;

                PersonInfo personData(PersonInfo::Id(), Person::Id(), ph_id, face_location);

                if (name.isEmpty() == false)
                {
                    // anounce new person, get id for it
                    const PersonName d(Person::Id(), name);
                    const Person::Id p_id = op->store(d);

                    // save representative photo
                    const QString path = QString("%1/%2.jpg").arg(base_path).arg(QString::number(p_id.value()));
                    face.save(path);

                    personData.p_id = p_id;
                }

                // store person information
                op->store(personData);
            });
        }
        else                                // someone known
        {
            const PersonInfo pinfo(PersonInfo::Id(), it->id(), m_id, face_coords);
            m_db->performCustomAction([pinfo]
                                      (Database::IBackendOperator* op)
            {
                // store person information
                op->store(pinfo);
            });
        }
    }

    // mark photo as analyzed
    m_db->performCustomAction([ph_id = m_id](Database::IBackendOperator* op)
    {
        op->set(ph_id, faces_recognized_flag, 1);
    });
}


std::vector<PersonName> FaceStore::fetchPeople()
{
    return evaluate<std::vector<PersonName>(Database::IBackendOperator* backend)>(m_db, [](Database::IBackendOperator* backend)
    {
        auto people = backend->listPeople();

        return people;
    });
}


///////////////////////////////////////////////////////////////////////////////


TestSystem::TestSystem(ICoreFactoryAccessor* core): m_core(core)
{
}


TestSystem::~TestSystem()
{
}


std::string TestSystem::name() const
{
    return "TestSystem";
}


void TestSystem::perform()
{
    FaceRecognition r(m_core);
    const QStringList modules = r.verifySystem();

    if (modules.empty())
        emit status(true, QString());
    else
    {
        const QString py_mods = modules.join(", ");
        const QString msg = tr("Automatic face detection and people recognition functionality is disabled due to missing Python modules.\n\n"
                               "To install them, use 'python -m pip install %1' (on Windows)\n"
                               "or use package system for your Linux distro.\n")
                            .arg(py_mods);

        emit status(false, msg);
    }
}


///////////////////////////////////////////////////////////////////////////////


PeopleOperator::PeopleOperator(const QString& storage, Database::IDatabase* db, ICoreFactoryAccessor* ca):
    m_storage(storage),
    m_db(db),
    m_coreFactory(ca)
{
    qRegisterMetaType<PersonName>("PersonData");
}


PeopleOperator::~PeopleOperator()
{
}


void PeopleOperator::testSystem() const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<TestSystem>(m_coreFactory);

    connect(task.get(), &TestSystem::status,
            this, &PeopleOperator::system_status);

    executor->addLight(std::move(task));
}


void PeopleOperator::fetchFaces(const Photo::Id& id) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FacesFetcher>(id, m_coreFactory, m_db);

    connect(task.get(), &FacesFetcher::faces,
            this, &PeopleOperator::faces);

    executor->addLight(std::move(task));
}


void PeopleOperator::recognize(const PeopleOperator::FaceLocation& face) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FaceRecognizer>(face, m_storage, m_coreFactory, m_db);

    connect(task.get(), &FaceRecognizer::recognized,
            this, &PeopleOperator::recognized);

    executor->addLight(std::move(task));
}


void PeopleOperator::getUnassignedPeople(const Photo::Id& id) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FetchUnassigned>(id, m_db);

    connect(task.get(), &FetchUnassigned::unassigned,
            this, &PeopleOperator::unassigned);

    executor->addLight(std::move(task));
}


void PeopleOperator::store(const Photo::Id& id,
                           const std::vector<FaceInfo>& known_people,
                           const QStringList& unknown_people) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FaceStore>(id, known_people, unknown_people, m_db, m_storage);

    executor->addLight(std::move(task));
}
