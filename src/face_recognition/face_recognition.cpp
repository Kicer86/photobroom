/*
 * Interface for face recognition.
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

#include "face_recognition.hpp"

#include <cassert>
#include <future>
#include <memory>
#include <string>

#undef slots
#include <pybind11/embed.h>

#include <QByteArray>
#include <QImage>
#include <QFileInfo>
#include <QString>
#include <QRect>
#include <QTemporaryFile>

#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>
#include <database/filter.hpp>
#include <system/filesystem.hpp>


namespace py = pybind11;
using namespace std::placeholders;

namespace
{
    QRect tupleToRect(const py::tuple& tuple)
    {
        QRect result;

        std::vector<long> rect;
        const std::size_t coordinates = tuple.size();

        if (coordinates == 4)
        {
            for(std::size_t i = 0; i < coordinates; i++)
            {
                auto part = tuple[i];

                const long n = part.cast<long>();
                rect.push_back(n);
            }

            const int top = rect[0];
            const int right = rect[1];
            const int bottom = rect[2];
            const int left = rect[3];
            const int width = right - left;
            const int height = bottom - top;

            result = QRect(left, top, width, height);
        }

        return result;
    }
}


FaceRecognition::FaceRecognition(ICoreFactoryAccessor* coreAccessor):
    m_pythonThread(coreAccessor->getPythonThread())
{

}


FaceRecognition::~FaceRecognition()
{

}


QVector<QRect> FaceRecognition::fetchFaces(const QString& path) const
{
    std::packaged_task<QVector<QRect>()> fetch_task([path]()
    {
        py::module find_faces = py::module::import("find_faces");
        py::object locations = find_faces.attr("find_faces")(path.toStdString());

        auto locations_list = locations.cast<py::list>();
        QVector<QRect> result;

        const std::size_t facesCount = locations_list.size();
        for(std::size_t i = 0; i < facesCount; i++)
        {
            auto item = locations_list[i];

            const QRect rect = tupleToRect(item.cast<py::tuple>());

            if (rect.isValid())
                result.push_back(rect);
        }

        return result;
    });

    auto fetch_future = fetch_task.get_future();
    m_pythonThread->execute(fetch_task);

    fetch_future.wait();

    return fetch_future.get();
}



QString FaceRecognition::recognize(const QString& path, const QRect& face, const QString& storage) const
{
    std::packaged_task<QString()> recognize_task([path, face, storage]()
    {
        QTemporaryFile tmpFile;

        const QImage photo(path);
        const QImage face_photo = photo.copy(face);
        face_photo.save(&tmpFile, "JPEG");

        py::module find_faces = py::module::import("recognize_face");
        py::object result = find_faces.attr("recognize_face")(tmpFile.fileName().toStdString(),
                                                              storage.toStdString());

        const std::string result_str = result.cast<py::str>();

        return QString::fromStdString(result_str);
    });

    auto recognize_future = recognize_task.get_future();
    m_pythonThread->execute(recognize_task);

    recognize_future.wait();

    return recognize_future.get();
}


/*
void FaceRecognition::store(const std::vector<std::pair<QRect, QString> >& people) const
{
    const QImage image(m_photo.path);

    for (const auto& person: people)
    {
        const QRect face_coords = person.first;
        const QString& name = person.second;

        auto it = std::find_if(m_data->m_people.cbegin(),
                               m_data->m_people.cend(),
                               [name](const PersonData& d)
        {
            return d.name() == name;
        });

        if (it == m_data->m_people.cend())  // we do not know that person
        {
            m_db->performCustomAction([photo_id = m_photo.id,
                                       name,
                                       base_path = m_data->m_storage,
                                       face = image.copy(face_coords),
                                       face_coords]
                                      (Database::IBackendOperator* op)
            {
                // anounce new face, get id for it
                const PersonData d(Person::Id(), name, "");
                const Person::Id p_id = op->store(d);

                // update face's path to representative
                const QString path = QString("%1/%2.jpg").arg(base_path).arg(QString::number(p_id.value()));
                const PersonData ud(p_id, name, path);

                // store face location
                op->store(photo_id, p_id, face_coords);

                op->store(ud);
                face.save(path);
            });
        }
        else                                // someone known
            m_db->performCustomAction([face_coords,
                                       photo_id = m_photo.id,
                                       p_id = it->id()]
                                      (Database::IBackendOperator* op)
            {
                // store face location
                op->store(photo_id, p_id, face_coords);
            });
    }
}
*/
