/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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

#include "people_manipulator.hpp"

#include <QFileInfo>

#include <core/icore_factory_accessor.hpp>
#include <core/task_executor_utils.hpp>
#include <database/ibackend.hpp>
#include <face_recognition/face_recognition.hpp>


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
    std::vector<QRect> fetchFacesFromDb(Database::IDatabase& db, Photo::Id id)
    {
        return evaluate<std::vector<QRect>(Database::IBackend*)>
            (&db, [id](Database::IBackend* backend)
        {
            std::vector<QRect> faces;

            const auto people = backend->listPeople(id);
            for(const auto& person: people)
                if (person.rect.isValid())
                    faces.push_back(person.rect);

            return faces;
        });
    }

    QString pathFor(Database::IDatabase* db, const Photo::Id& id)
    {
        return evaluate<QString(Database::IBackend *)>(db, [id, db](Database::IBackend *)
        {
            Database::IUtils* db_utils = db->utils();
            auto photo = db_utils->getPhotoFor(id);

            return photo->getPath();
        });
    }
}



PeopleManipulator::PeopleManipulator(const Photo::Id& pid, Database::IDatabase& db, ICoreFactoryAccessor& core)
    : m_pid(pid)
    , m_core(core)
    , m_db(db)
{
    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");

    findFaces();
}


void PeopleManipulator::findFaces()
{
    auto task = std::bind(&PeopleManipulator::findFaces_thrd, this);
    auto safe_task = m_callback_ctrl.make_safe_callback<void()>(task);
    auto executor = m_core.getTaskExecutor();

    runOn(executor, safe_task);
}


void PeopleManipulator::findFaces_thrd()
{
    QVector<QRect> result;
    const std::vector<QRect> list_of_faces = fetchFacesFromDb(m_db, m_pid);

    if (list_of_faces.empty())
    {
        const QString path = pathFor(&m_db, m_pid);
        const QFileInfo pathInfo(path);
        const QString full_path = pathInfo.absoluteFilePath();

        FaceRecognition face_recognition(&m_core);
        const auto faces = face_recognition.fetchFaces(full_path);

        for(const QRect& face: faces)
            result.append(face);
    }
    else
    {
        result.reserve(static_cast<int>(list_of_faces.size()));

        std::copy(list_of_faces.cbegin(), list_of_faces.cend(), std::back_inserter(result));
    }

    invokeMethod(this, &PeopleManipulator::findFaces_result, result);
}


void PeopleManipulator::findFaces_result(const QVector<QRect>& faces)
{
    m_faces.reserve(faces.size());

    std::copy(faces.cbegin(), faces.cend(), std::back_inserter(m_faces));
}
