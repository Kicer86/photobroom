
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
#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/map_iterator.hpp>
#include <core/task_executor_utils.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_operator.hpp"


FaceOptimizer::FaceOptimizer(Database::IDatabase* db,
                             ICoreFactoryAccessor* core,
                             PeopleOperator* op):
    m_operator(op),
    m_tmpDir(System::getTmpDir("FaceOptimizer")),
    m_db(db),
    m_core(core)
{
}


FaceOptimizer::~FaceOptimizer()
{
    m_safe_callback.invalidate();
}


void FaceOptimizer::set(const std::map<Photo::Id, QString>& paths)
{
    m_photo2path = paths;
}


void FaceOptimizer::findBest(const std::vector<PersonInfo>& pis,
                             const std::function<void(const QString &)>& callback)
{
    auto task = [this, callback, pis]
    {
        FaceRecognition face_recognition(m_core);

        const auto path2Person = saveFiles(pis);

        QStringList files;
        std::copy(key_map_iterator<decltype(path2Person)>(path2Person.cbegin()),
                  key_map_iterator<decltype(path2Person)>(path2Person.cend()),
                  std::back_inserter(files));

        const auto best_face_path = face_recognition.best(files);

        auto it = path2Person.find(best_face_path);

        assert(it != path2Person.cend() || best_face_path.isEmpty()); // if `best_face_path` isn't empty, then we should find person

        if (it == path2Person.cend())
            callback(QString());
        else
        {
            const PersonInfo& pi = it->second;
            m_operator->setModelFaceSync(pi);

            const QString path = m_operator->getModelFace(pi.p_id);
            callback(path);
        }
    };

    auto safe_task = m_safe_callback.make_safe_callback<void()>(task);
    auto* taskMgr = m_core->getTaskExecutor();

    runOn(taskMgr, safe_task);
}


QString FaceOptimizer::current(const Person::Id& id) const
{
    return m_operator->getModelFace(id);
}


std::map<QString, PersonInfo> FaceOptimizer::saveFiles(const std::vector<PersonInfo>& pis)
{
    std::map<QString, PersonInfo> results;
    for(const PersonInfo& pi: pis)
    {
        auto it = m_photo2path.find(pi.ph_id);

        assert(it != m_photo2path.end());

        if (it != m_photo2path.end())
        {
            const QString& path = it->second;
            const QRect& faceRect = pi.rect;
            const QImage photo = Image::normalized(path, m_core->getExifReaderFactory()->get());
            const QImage face = photo.copy(faceRect);
            const QString file_path = System::getTmpFile(m_tmpDir->path(), "jpeg");

            face.save(file_path);

            results.emplace(file_path, pi);
        }
    }

    return results;
}
