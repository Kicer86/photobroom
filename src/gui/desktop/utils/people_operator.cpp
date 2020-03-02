/*
 * Photo Broom - photos management tool.
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

#include <QFile>
#include <QFileInfo>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/image_tools.hpp>
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
        db->exec(std::forward<T>(t));
    }
};

namespace
{
    const QString faces_recognized_flag = QStringLiteral("faces_recognized");

    QString pathFor(Database::IDatabase* db, const Photo::Id& id)
    {
        return evaluate<QString(Database::IBackend *)>(db, [id, db](Database::IBackend *)
        {
            Database::IUtils* db_utils = db->utils();
            auto photo = db_utils->getPhotoFor(id);

            return photo->getPath();
        });
    }

    QImage imageFor(Database::IDatabase* db, const PersonInfo& pi, IExifReaderFactory* exifFactory)
    {
        const QString path = pathFor(db, pi.ph_id);
        const QRect& faceRect = pi.rect;
        const OrientedImage photo = Image::normalized(path, exifFactory->get());
        const QImage faceImg = photo->copy(faceRect);

        return faceImg;
    }

    void storeNewPersonInDb(const PersonInfo& pi, Database::IDatabase* db, ICoreFactoryAccessor* coreAccessor, const QString& name, Database::IBackend* op)
    {
        PersonInfo personInfo = pi;

        if (name.isEmpty() == false)
        {
            // anounce new person, get id for it
            const PersonName d(Person::Id(), name);
            personInfo.p_id = op->store(d);

            // save representative photo
            ModelFaceStore mfs(personInfo, db, coreAccessor);
            mfs.perform();
        }

        // store person information
        op->store(personInfo);
    }
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
    return evaluate<std::vector<QRect>(Database::IBackend*)>
        (m_db, [id = m_id](Database::IBackend* backend)
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
    return evaluate<QString(Database::IBackend *)>(m_db, [this, id = m_id](Database::IBackend *)
    {
        Database::IUtils* db_utils = m_db->utils();
        auto photo = db_utils->getPhotoFor(id);

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
        const QString path = pathFor(m_db, m_id);
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


FaceRecognizer::FaceRecognizer(const PeopleOperator::FaceLocation& face, ICoreFactoryAccessor* core, Database::IDatabase* db):
    FaceTask(face.first, db),
    m_logger(core->getLoggerFactory()->get("FaceRecognizer")),
    m_data(face),
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
        const Person::Id pid = face_recognition.recognize(full_path, m_data.second, m_db);

        if (pid.valid())
        {
            result = personData(pid);

            const QString msg = QString("%1 recognized on photo").arg(result.name());
            m_logger->debug(msg);
        }
    }

    emit recognized(m_data.second, result);
}


PersonName FaceRecognizer::personData(const Person::Id& id) const
{
    const PersonName person =
        evaluate<PersonName (Database::IBackend *)>
            (m_db, [id](Database::IBackend* backend)
    {
        const auto people = backend->person(id);

        return people;
    });

    return person;
}


std::vector<PersonInfo> FaceRecognizer::fetchPeopleFromDb() const
{
    return evaluate<std::vector<PersonInfo>(Database::IBackend *)>(m_db, [id = m_id](Database::IBackend* backend)
    {
        auto people = backend->listPeople(id);

        return people;
    });
}


bool FaceRecognizer::wasAnalyzed() const
{
    return evaluate<bool(Database::IBackend *)>(m_db, [id = m_id](Database::IBackend* backend)
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
    const QStringList un = evaluate<QStringList(Database::IBackend*)>(m_db, [id = m_id](Database::IBackend* backend)
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
                     ICoreFactoryAccessor* coreAccessor):
    FaceTask(id, db),
    m_knownPeople(known_people),
    m_unknownPeople(unknown_people),
    m_coreAccessor(coreAccessor)
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

    // store people data
    for (const auto& person: m_knownPeople )
    {
        const QString& name = person.name;
        const QRect& face_coords = person.rect;

        auto it = std::find_if(people.cbegin(),
                               people.cend(),
                               [name](const PersonName& d)
        {
            return d.name() == name;
        });

        if (it == people.cend())  // we do not know that person
        {
            const PersonInfo pi(Person::Id(), m_id, {}, face_coords);

            auto storeNewPersonDetails = std::bind(storeNewPersonInDb, pi, m_db, m_coreAccessor, name, _1);
            m_db->exec(storeNewPersonDetails);
        }
        else                                // someone known
        {
            const PersonInfo pinfo(it->id(), m_id, {}, face_coords);
            m_db->exec([pinfo](Database::IBackend* backend)
            {
                // store person information
                backend->store(pinfo);
            });
        }
    }

    // mark photo as analyzed
    m_db->exec([ph_id = m_id](Database::IBackend* op)
    {
        op->set(ph_id, faces_recognized_flag, 1);
    });
}


std::vector<PersonName> FaceStore::fetchPeople()
{
    return evaluate<std::vector<PersonName>(Database::IBackend *)>(m_db, [](Database::IBackend* backend)
    {
        auto people = backend->listPeople();

        return people;
    });
}


///////////////////////////////////////////////////////////////////////////////


ModelFaceStore::ModelFaceStore(const PersonInfo& pi,
                               Database::IDatabase* db,
                               ICoreFactoryAccessor* coreAccessor
                              ):
    FaceTask(pi.ph_id, db),
    m_pi(pi),
    m_coreAccessor(coreAccessor)
{
}


std::string ModelFaceStore::name() const
{
    return "ModelFaceStore";
}


void ModelFaceStore::perform()
{
    const QString photo_path = getPhotoPath();
    const OrientedImage image = Image::normalized(photo_path, m_coreAccessor->getExifReaderFactory()->get());
    const QImage face = image->copy(m_pi.rect);

    assert(!"reimplement");

    //const QString face_path = QString("%1/%2.jpg").arg(m_storage).arg(QString::number(m_pi.p_id.value()));
    //face.save(face_path);

    emit done(m_pi.p_id);
}


///////////////////////////////////////////////////////////////////////////////


PeopleOperator::PeopleOperator(Database::IDatabase* db, ICoreFactoryAccessor* ca):
    m_db(db),
    m_coreFactory(ca)
{
    qRegisterMetaType<PersonName>("PersonData");
    qRegisterMetaType<Person::Id>("Person::Id");
}


PeopleOperator::~PeopleOperator()
{
}


void PeopleOperator::findFaces(const Photo::Id& id) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FacesFetcher>(id, m_coreFactory, m_db);

    connect(task.get(), &FacesFetcher::faces,
            this, &PeopleOperator::faces);

    executor->add(std::move(task));
}


void PeopleOperator::recognize(const PeopleOperator::FaceLocation& face) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FaceRecognizer>(face, m_coreFactory, m_db);

    connect(task.get(), &FaceRecognizer::recognized,
            this, &PeopleOperator::recognized);

    executor->add(std::move(task));
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
    auto task = std::make_unique<FaceStore>(id, known_people, unknown_people, m_db, m_coreFactory);

    executor->addLight(std::move(task));
}


QString PeopleOperator::getModelFace(const Person::Id& p_id) const
{
    assert(p_id.valid());

    assert(!"reimplement");

    return "";
}


void PeopleOperator::setModelFace(const PersonInfo& pi)
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<ModelFaceStore>(pi, m_db, m_coreFactory);

    connect(task.get(), &ModelFaceStore::done,
            this, &PeopleOperator::modelFaceSet);

    executor->add(std::move(task));
}


void PeopleOperator::setModelFaceSync(const PersonInfo& pi)
{
    ModelFaceStore(pi, m_db, m_coreFactory).perform();
}


void PeopleOperator::getFace(const PersonInfo& pi, const std::function<void(const QImage &)>& callback)
{
    IExifReaderFactory* exifFactory = m_coreFactory->getExifReaderFactory();
    runOn(m_coreFactory->getTaskExecutor(), [db = m_db, pi, callback, exifFactory]
    {
        const QImage faceImg = imageFor(db, pi, exifFactory);

        callback(faceImg);
    });
}


QImage PeopleOperator::getFaceSync(const PersonInfo& pi)
{
    return imageFor(m_db, pi, m_coreFactory->getExifReaderFactory());
}

