/*
 * helper for finding best face
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

#include "face_optimizer.hpp"

#include <core/icore_factory_accessor.hpp>
#include <core/map_iterator.hpp>
#include <core/task_executor_utils.hpp>
#include <face_recognition/face_recognition.hpp>


FaceOptimizer::FaceOptimizer(Database::IDatabase* db,
                             ICoreFactoryAccessor* core):
    m_tmpDir(System::getTmpDir("FaceOptimizer")),
    m_db(db),
    m_core(core)
{
}


FaceOptimizer::~FaceOptimizer()
{
    m_safe_callback.invalidate();
}


void FaceOptimizer::optimize(const Person::Id& pid,
                             const std::vector<PersonInfo>& pis,
                             const std::map<Photo::Id, QString>& paths)
{
    auto task = [this, pid, pis, paths]
    {
        FaceRecognition face_recognition(m_core);

        const auto path2Person = saveFiles(pis, paths);

        QStringList files;
        std::copy(key_map_iterator<decltype(path2Person)>(path2Person.cbegin()),
                  key_map_iterator<decltype(path2Person)>(path2Person.cend()),
                  std::back_inserter(files));

        const auto best_face_path = face_recognition.best(files);

        auto it = path2Person.find(best_face_path);

        assert(it != path2Person.cend() || best_face_path.isEmpty()); // if `best_face_path` isn't empty, then we should find person

        if (it == path2Person.cend())
            emit error(pid);
        else
            emit best(it->second);
    };

    auto safe_task = m_safe_callback.make_safe_callback<void()>(task);
    auto* taskMgr = m_core->getTaskExecutor();

    runOn(taskMgr, safe_task);
}


void FaceOptimizer::findBest(const std::vector<PersonInfo>& pis,
                             const std::map<Photo::Id, QString>& paths,
                             const std::function<void (const PersonInfo &)>& callback)
{
    auto task = [this, callback, pis, paths]
    {
        static PersonInfo invalid;

        FaceRecognition face_recognition(m_core);

        const auto path2Person = saveFiles(pis, paths);

        QStringList files;
        std::copy(key_map_iterator<decltype(path2Person)>(path2Person.cbegin()),
                  key_map_iterator<decltype(path2Person)>(path2Person.cend()),
                  std::back_inserter(files));

        const auto best_face_path = face_recognition.best(files);

        auto it = path2Person.find(best_face_path);

        assert(it != path2Person.cend() || best_face_path.isEmpty()); // if `best_face_path` isn't empty, then we should find person

        if (it == path2Person.cend())
            callback(invalid);
        else
            callback(it->second);
    };

    auto safe_task = m_safe_callback.make_safe_callback<void()>(task);
    auto* taskMgr = m_core->getTaskExecutor();

    runOn(taskMgr, safe_task);
}



std::map<QString, PersonInfo> FaceOptimizer::saveFiles(const std::vector<PersonInfo>& pis,
                                                       const std::map<Photo::Id, QString>& paths)
{
    std::map<QString, PersonInfo> results;
    for(const PersonInfo& pi: pis)
    {
        auto it = paths.find(pi.ph_id);

        assert(it != paths.end());

        if (it != paths.end())
        {
            const QString& path = it->second;
            const QRect& faceRect = pi.rect;
            const QImage photo(path);
            const QImage face = photo.copy(faceRect);
            const QString file_path = System::getTmpFile(m_tmpDir->path(), "jpeg");

            face.save(file_path);

            results.emplace(file_path, pi);
        }
    }

    return results;
}
