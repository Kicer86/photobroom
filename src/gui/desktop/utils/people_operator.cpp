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
#include <database/idatabase.hpp>
#include <face_recognition/face_recognition.hpp>

#include "people_operator_p.hpp"


using namespace std::placeholders;

template<typename T>
struct ExecutorTraits<Database::IDatabase, T>
{
    static void exec(Database::IDatabase* db, T&& t)
    {
        db->performCustomAction(std::forward<T>(t));
    }
};


FaceTask::FaceTask(const Photo::Id& id, Database::IDatabase* d):
    m_id(id),
    m_db(d)
{

}


FaceTask::~FaceTask()
{

}


std::vector<FaceData> FaceTask::fetchFacesFromDb() const
{
    return evaluate<std::vector<FaceData>(Database::IBackendOperator*)>
        (m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        return backend->listFaces(id);
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
    qRegisterMetaType<QVector<FaceData>>("QVector<FaceData>");
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
    QVector<FaceData> result;
    const std::vector<FaceData> list_of_faces = fetchFacesFromDb();

    if (list_of_faces.empty())
    {
        const QString path = getPhotoPath();
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(m_coreFactory);
        const auto faces = face_recognition.fetchFaces(full_path);

        for(const QRect& face: faces)
            result.append(FaceData(Face::Id(), m_id, face));
    }
    else
    {
        result.reserve(static_cast<int>(list_of_faces.size()));

        std::copy(list_of_faces.cbegin(), list_of_faces.cend(), std::back_inserter(result));
    }

    emit faces(result);
}


///////////////////////////////////////////////////////////////////////////////


FaceRecognizer::FaceRecognizer(const Photo::Id& id, const QRect& rect, const QString& patterns, ICoreFactoryAccessor* core, Database::IDatabase* db):
    FaceTask(id, db),
    m_rect(rect),
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
    const std::vector<PersonLocation> peopleData = fetchPeopleFromDb();
    PersonData result;

    if (peopleData.empty())
    {
        const QString path = getPhotoPath();
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(m_coreFactory);
        const QString personPath = face_recognition.recognize(full_path, m_rect, m_patterns);

        if (personPath.isEmpty() == false)
        {
            const QFileInfo pathInfo(personPath);
            const QString personId = pathInfo.baseName();
            const Person::Id pid(personId.toInt());
            result = personData(pid);
        }
    }
    else
        for(const PersonLocation& location: peopleData)
            if (location.location == m_rect)
            {
                result = personData(location.id);
                break;
            }

    emit recognized(result);
}


PersonData FaceRecognizer::personData(const Person::Id& id) const
{
    const PersonData person =
        evaluate<PersonData(Database::IBackendOperator *)>
            (m_db, [id](Database::IBackendOperator* backend)
    {
        const auto people = backend->person(id);

        return people;
    });

    return person;
}


std::vector<PersonLocation> FaceRecognizer::fetchPeopleFromDb() const
{
    return evaluate<std::vector<PersonLocation>(Database::IBackendOperator* backend)>(m_db, [id = m_id](Database::IBackendOperator* backend)
    {
        auto people = backend->listPeople(id);

        return people;
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
        auto photo = backend->getPhotoFor(id);
        const Tag::TagsList tags = photo->getTags();

        QStringList people;
        for(const std::pair<TagNameInfo, TagValue>& tag: tags)
        {
            if (tag.first.getTag() == BaseTagsList::People)
            {
                const std::vector<TagValue> subtags = tag.second.getList();

                for(const TagValue& subtag: subtags)
                    people.append(subtag.getString());
            }
        }

        const std::vector<PersonLocation> locations = backend->listPeople(id);
        QStringList locatedPeople;

        for(const PersonLocation& location: locations)
        {
            const PersonData data = backend->person(location.id);
            locatedPeople.append(data.name());
        }

        people.sort();
        locatedPeople.sort();

        QStringList unassigned;
        std::set_difference(people.begin(), people.end(),
                            locatedPeople.begin(), locatedPeople.end(),
                            std::back_inserter(unassigned));

        return unassigned;
    });

    emit unassigned(un);
}


///////////////////////////////////////////////////////////////////////////////


FaceStore::FaceStore(const Photo::Id& id, const std::vector<std::pair<FaceData, QString>>& data, Database::IDatabase* db, const QString& patterns):
    FaceTask(id, db),
    m_data(data),
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
    const std::vector<PersonData> people = fetchPeople();
    const QString path = getPhotoPath();
    const QImage image(path);

    for (const auto& person: m_data)
    {
        const FaceData& faceData = person.first;
        const QString& name = person.second;
        const QRect& face_coords = faceData.rect;

        auto it = std::find_if(people.cbegin(),
                               people.cend(),
                               [name](const PersonData& d)
        {
            return d.name() == name;
        });

        if (it == people.cend())  // we do not know that person
        {
            m_db->performCustomAction([photo_id = m_id,
                                       name,
                                       base_path = m_patterns,
                                       face = image.copy(face_coords),
                                       faceData]
                                      (Database::IBackendOperator* op)
            {

                // store face location
                const Face::Id f_id = op->store(faceData);

                if (name.isEmpty() == false)
                {
                    // anounce new person, get id for it
                    const PersonData d(Person::Id(), name);
                    const Person::Id p_id = op->store(d);

                    // save representative photo
                    const QString path = QString("%1/%2.jpg").arg(base_path).arg(QString::number(p_id.value()));
                    face.save(path);

                    // store person location
                    op->store(p_id, f_id);
                }
            });
        }
        else                                // someone known
            m_db->performCustomAction([faceData,
                                       p_id = it->id()]
                                      (Database::IBackendOperator* op)
            {
                // store face location
                const Face::Id f_id = op->store(faceData);

                // store person location
                op->store(p_id, f_id);
            });
    }
}


std::vector<PersonData> FaceStore::fetchPeople()
{
    return evaluate<std::vector<PersonData>(Database::IBackendOperator* backend)>(m_db, [](Database::IBackendOperator* backend)
    {
        auto people = backend->listPeople();

        return people;
    });
}


///////////////////////////////////////////////////////////////////////////////


PeopleOperator::PeopleOperator(const QString& storage, Database::IDatabase* db, ICoreFactoryAccessor* ca):
    m_storage(storage),
    m_db(db),
    m_coreFactory(ca)
{
    qRegisterMetaType<PersonData>("PersonData");
    qRegisterMetaType<FaceData>("FaceData");
}


PeopleOperator::~PeopleOperator()
{
}


void PeopleOperator::fetchFaces(const Photo::Id& id) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FacesFetcher>(id, m_coreFactory, m_db);

    auto notifier = std::bind(&PeopleOperator::faces, this, id, _1);
    connect(task.get(), &FacesFetcher::faces, notifier);

    executor->add(std::move(task)); // TODO: this task will mostly wait. Use new mechanism (issue #247)
}


void PeopleOperator::recognize(const Photo::Id& id, const FaceData& face) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FaceRecognizer>(id, face.rect, m_storage, m_coreFactory, m_db);

    auto notifier = std::bind(&PeopleOperator::recognized, this, id, face, _1);
    connect(task.get(), &FaceRecognizer::recognized, notifier);

    executor->add(std::move(task)); // TODO: this task will mostly wait. Use new mechanism (issue #247)
}


void PeopleOperator::getUnassignedPeople(const Photo::Id& id) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FetchUnassigned>(id, m_db);

    auto notifier = std::bind(&PeopleOperator::unassigned, this, id, _1);
    connect(task.get(), &FetchUnassigned::unassigned, notifier);

    executor->add(std::move(task)); // TODO: this task will mostly wait. Use new mechanism (issue #247)
}


void PeopleOperator::store(const Photo::Id& id, const std::vector<std::pair<FaceData, QString> >& people) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FaceStore>(id, people, m_db, m_storage);

    executor->add(std::move(task)); // TODO: this task will mostly wait. Use new mechanism (issue #247)
}
