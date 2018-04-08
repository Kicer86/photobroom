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
#include <memory>
#include <string>

#include <pybind11/embed.h>

#include <QByteArray>
#include <QImage>
#include <QFileInfo>
#include <QString>
#include <QRect>
#include <QTemporaryFile>

#include <core/icore_factory_accessor.hpp>
#include <core/ipython_thread.hpp>
#include <database/idatabase.hpp>
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


struct FaceRecognition::SharedData
{
    const QString m_storage;
    std::vector<PersonData> m_people;
    std::atomic<bool> m_peopleLoaded;

    SharedData(const QString& storage):
        m_storage(storage),
        m_peopleLoaded(false)
    {

    }
};


FaceRecognition::FaceRecognition(ICoreFactoryAccessor* coreAccessor, Database::IDatabase* db, const QString& storage):
    m_data(new SharedData(storage)),
    m_pythonThread(coreAccessor->getPythonThread()),
    m_db(db)
{
    auto load = std::bind(&FaceRecognition::loadData, this, _1);
    m_db->performCustomAction(load);
}


FaceRecognition::~FaceRecognition()
{

}


void FaceRecognition::findFaces(const QString& photo, const Callback<const QVector<QRect> &>& callback) const
{
    m_pythonThread->execute([photo, callback]()
    {
        int status = 0;

        py::module find_faces = py::module::import("find_faces");
        py::object locations = find_faces.attr("find_faces")(photo.toStdString());

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

        callback(result);
    });
}


void FaceRecognition::nameFor(const QString& path, const QRect& face, const Callback<const QString &>& callback) const
{
    auto data = m_data;
    m_pythonThread->execute([path, face, data, callback]()
    {
        QTemporaryFile tmpFile;

        const QImage photo(path);
        const QImage face_photo = photo.copy(face);
        face_photo.save(&tmpFile, "JPEG");

        py::module find_faces = py::module::import("recognize_face");
        py::object result = find_faces.attr("recognize_face")(tmpFile.fileName().toStdString(),
                                                              data->m_storage.toStdString());

        const std::string result_str = result.cast<py::str>();

        QString name;

        if (result_str.empty() == false)
        {
            const QFileInfo fileInfo(result_str.c_str());
            const QByteArray& file_name = fileInfo.baseName().toUtf8();
            const int id = file_name.toInt();

            auto it = std::find_if(data->m_people.cbegin(),
                                   data->m_people.cend(),
                                   [id](const PersonData& d)
            {
                return d.id() == id;
            });

            if (it != data->m_people.cend())
                name = it->name();
        }

        callback(name);
    });
}


void FaceRecognition::store(const QString& photo, const std::vector<std::pair<QRect, QString> >& people) const
{
    const QImage image(photo);

    for (const auto& person: people)
    {
        const QString& name = person.second;
        auto it = std::find_if(m_data->m_people.cbegin(),
                               m_data->m_people.cend(),
                               [name](const PersonData& d)
        {
            return d.name() == name;
        });

        if (it == m_data->m_people.cend())  // we do not have it
        {
            const QImage face = image.copy(person.first);
            const QString base_path = m_data->m_storage;

            m_db->performCustomAction([name, base_path, face](Database::IBackendOperator* op)
            {
                // anounce new face, get id for it
                const PersonData d(Person::Id(), name, "");
                const Person::Id id = op->store(d);

                // update face's path to representative
                const QString path = QString("%1/%2.jpg").arg(base_path).arg(QString::number(id.value()));
                const PersonData ud(id, name, path);

                op->store(ud);
                face.save(path);
            });
        }
    }
}


void FaceRecognition::loadData(Database::IBackendOperator* op)
{
    m_data->m_people = op->listPeople();
    m_data->m_peopleLoaded = true;
}
