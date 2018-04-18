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
    std::vector<PersonLocation> peopleData = fetchFacesFromDb();

    if (peopleData.empty())
    {
        const QString path = getPhotoPath();
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(Photo::Data(), m_coreFactory, m_db, "");
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
        db_task2([id = m_id](Database::IBackendOperator* backend)
    {
        auto photo = backend->getPhotoFor(id);

        return photo->getPath();
    });

    auto result_future2 = db_task2.get_future();
    m_db->performCustomAction(std::move(db_task2));

    result_future2.wait();

    return result_future2.get();
}


///////////////////////////////////////////////////////////////////////////////


PeopleOperator::PeopleOperator(Database::IDatabase* db, ICoreFactoryAccessor* ca):
    m_db(db),
    m_coreFactory(ca)
{
}


PeopleOperator::~PeopleOperator()
{
}


void PeopleOperator::fetchFaces(const Photo::Id& photo) const
{
    ITaskExecutor* executor = m_coreFactory->getTaskExecutor();
    auto task = std::make_unique<FacesFetcher>(photo, m_coreFactory, m_db);

    auto notifier = std::bind(&PeopleOperator::faces, this, photo, _1);
    connect(task.get(), &FacesFetcher::faces, notifier);

    executor->add(std::move(task)); // TODO: this task will mostly wait. Use new mechanism (issue #247)
}
