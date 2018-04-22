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
#include <future>

#include <QFileInfo>

#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <face_recognition/face_recognition.hpp>

#include "people_operator_p.hpp"


using namespace std::placeholders;


FacesFetcher::FacesFetcher(const Photo::Id& id, ICoreFactoryAccessor* f, Database::IDatabase* d):
    m_id(id),
    m_coreFactory(f),
    m_db(d)
{

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
    const std::vector<PersonLocation> peopleData = fetchFacesFromDb();

    if (peopleData.empty())
    {
        const QString path = getPhotoPath();
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(m_coreFactory);
        result = face_recognition.fetchFaces(full_path);
    }
    else
    {
        result.reserve(peopleData.size());

        for (const PersonLocation& loc: peopleData )
            result.push_back(loc.location);
    }

    emit faces(result);
}


std::vector<PersonLocation> FacesFetcher::fetchFacesFromDb() const
{
    std::packaged_task<std::vector<PersonLocation>(Database::IBackendOperator*)>
        db_task([id = m_id](Database::IBackendOperator* backend)
    {
        return backend->listFaces(id);
    });

    auto result_future = db_task.get_future();

    m_db->performCustomAction(std::move(db_task));
    result_future.wait();

    return result_future.get();
}


QString FacesFetcher::getPhotoPath() const
{
    std::packaged_task<QString(Database::IBackendOperator* backend)>
        db_task([id = m_id](Database::IBackendOperator* backend)
    {
        auto photo = backend->getPhotoFor(id);

        return photo->getPath();
    });

    auto result_future = db_task.get_future();
    m_db->performCustomAction(std::move(db_task));

    result_future.wait();

    return result_future.get();
}


///////////////////////////////////////////////////////////////////////////////


FaceRecognizer::FaceRecognizer(const Photo::Id& id, const QRect& rect, const QString& patterns, ICoreFactoryAccessor* core, Database::IDatabase* db):
    m_id(id),
    m_rect(rect),
    m_patterns(patterns),
    m_coreFactory(core),
    m_db(db)
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
    const std::vector<PersonLocation> peopleData = fetchFacesFromDb();
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


std::vector<PersonLocation> FaceRecognizer::fetchFacesFromDb() const
{
    std::packaged_task<std::vector<PersonLocation>(Database::IBackendOperator*)>
        db_task([id = m_id](Database::IBackendOperator* backend)
    {
        return backend->listFaces(id);
    });

    auto result_future = db_task.get_future();

    m_db->performCustomAction(std::move(db_task));
    result_future.wait();

    return result_future.get();
}


QString FaceRecognizer::getPhotoPath() const
{
    std::packaged_task<QString(Database::IBackendOperator* backend)>
        db_task([id = m_id](Database::IBackendOperator* backend)
    {
        const auto photo = backend->getPhotoFor(id);

        return photo->getPath();
    });

    auto result_future = db_task.get_future();
    m_db->performCustomAction(std::move(db_task));

    result_future.wait();

    return result_future.get();
}


PersonData FaceRecognizer::personData(const Person::Id& id) const
{
    std::packaged_task<std::vector<PersonData>(Database::IBackendOperator* backend)>
        db_task([](Database::IBackendOperator* backend)
    {
        const auto people = backend->listPeople();

        return people;
    });

    auto result_future = db_task.get_future();
    m_db->performCustomAction(std::move(db_task));

    result_future.wait();

    const auto people = result_future.get();

    PersonData result;
    for(const PersonData& person: people)
        if (person.id() == id)
        {
            result = person;
            break;
        }

    return result;
}


///////////////////////////////////////////////////////////////////////////////


PeopleOperator::PeopleOperator(const QString& storage, Database::IDatabase* db, ICoreFactoryAccessor* ca):
    m_storage(storage),
    m_db(db),
    m_coreFactory(ca)
{
    qRegisterMetaType<PersonData>("PersonData");
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


void PeopleOperator::recognize(const Photo::Id& id, const QRect& face) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FaceRecognizer>(id, face, m_storage, m_coreFactory, m_db);

    auto notifier = std::bind(&PeopleOperator::recognized, this, id, face, _1);
    connect(task.get(), &FaceRecognizer::recognized, notifier);

    executor->add(std::move(task)); // TODO: this task will mostly wait. Use new mechanism (issue #247)
}
