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

#include "face_optimizer.hpp"

#include <core/icore_factory_accessor.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/map_iterator.hpp>
#include <core/task_executor_utils.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_operator.hpp"


FaceOptimizer::FaceOptimizer(ICoreFactoryAccessor* core,
                             PeopleOperator* op):
    m_logger(core->getLoggerFactory()->get("FaceOptimizer")),
    m_tmpDir(System::createTmpDir("FaceOptimizer", System::Confidential)),
    m_operator(op),
    m_core(core)
{
}


FaceOptimizer::~FaceOptimizer()
{
    m_safe_callback.invalidate();
}


void FaceOptimizer::findBest(const std::vector<PersonInfo>& pis,
                             const std::function<void(const QString &)>& callback)
{
    auto task = [this, callback, pis]
    {
        FaceRecognition face_recognition(m_core);

        const auto path2Person = saveFiles(pis);

        QStringList files;
        for (const auto& d : path2Person)
            files.append(d.first);

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


QString FaceOptimizer::currentBest(const Person::Id& id) const
{
    return m_operator->getModelFace(id);
}


std::map<QString, PersonInfo> FaceOptimizer::saveFiles(const std::vector<PersonInfo>& pis)
{
    std::map<QString, PersonInfo> results;
    for(const PersonInfo& pi: pis)
    {
        const QImage face = m_operator->getFaceSync(pi);

        if (face.isNull() == false)
        {
            const QString file_path = System::getTmpFile(m_tmpDir->path(), "jpeg");

            face.save(file_path);

            results.emplace(file_path, pi);
        }
        else
        {
            const QRect& faceRect = pi.rect;
            const QPoint& faceCenter = faceRect.center();
            const QString msg = QString("Error when accessing face for optimization. "
                                        "Photo id: %1, person id: %2, face location: %3x%4")
                                .arg(pi.ph_id)
                                .arg(pi.p_id)
                                .arg(faceCenter.x())
                                .arg(faceCenter.y());

            m_logger->error(msg);
        }
    }

    return results;
}
