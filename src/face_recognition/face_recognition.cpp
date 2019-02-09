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

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <QByteArray>
#include <QImage>
#include <QFileInfo>
#include <QString>
#include <QRect>
#include <QTemporaryFile>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/image_tools.hpp>
#include <core/ipython_thread.hpp>
#include <database/filter.hpp>
#include <system/filesystem.hpp>
#include <system/system.hpp>


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

    QStringList missingModules()
    {
        QStringList result;

        try
        {
            py::module system_test = py::module::import("system_test");
            py::object missing = system_test.attr("detect_required_modules")();

            auto missing_list = missing.cast<py::list>();

            const std::size_t count = missing_list.size();
            for (std::size_t i = 0; i < count; i++)
            {
                auto item = missing_list[i];

                const std::string missing_module = item.cast<std::string>();
                result.append(missing_module.c_str());
            }
        }
        catch (const std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
        }

        return result;
    }
}


FaceRecognition::FaceRecognition(ICoreFactoryAccessor* coreAccessor):
    m_tmpDir(System::getTmpDir("FaceRecognition")),
    m_pythonThread(coreAccessor->getPythonThread()),
    m_exif(coreAccessor->getExifReaderFactory()->get())
{

}


FaceRecognition::~FaceRecognition()
{

}


QStringList FaceRecognition::verifySystem() const
{
    std::packaged_task<QStringList()> test_task([]()
    {
        return missingModules();
    });

    auto test_future = test_task.get_future();
    m_pythonThread->execute(test_task);

    test_future.wait();

    return test_future.get();
}


QVector<QRect> FaceRecognition::fetchFaces(const QString& path) const
{
    QVector<QRect> result;

    const QString normalizedPhotoPath = System::getTmpFile(m_tmpDir->path(), "jpeg");
    const bool s = Image::normalize(path, normalizedPhotoPath, m_exif);

    if (s)
    {
        std::packaged_task<QVector<QRect>()> fetch_task([path = normalizedPhotoPath]()
        {
            QVector<QRect> result;
            const QStringList mm = missingModules();

            if (mm.empty())
            {
                py::module find_faces = py::module::import("find_faces");
                py::object locations = find_faces.attr("find_faces")(path.toStdString());

                auto locations_list = locations.cast<py::list>();

                const std::size_t facesCount = locations_list.size();
                for(std::size_t i = 0; i < facesCount; i++)
                {
                    auto item = locations_list[i];

                    const QRect rect = tupleToRect(item.cast<py::tuple>());

                    if (rect.isValid())
                        result.push_back(rect);
                }
            }

            return result;
        });

        auto fetch_future = fetch_task.get_future();
        m_pythonThread->execute(fetch_task);

        fetch_future.wait();

        result = fetch_future.get();
    }

    return result;
}


QString FaceRecognition::recognize(const QString& path, const QRect& face, const QString& storage) const
{
    const QString normalizedPhotoPath = System::getTmpFile(m_tmpDir->path(), "jpeg");
    Image::normalize(path, normalizedPhotoPath, m_exif);

    std::packaged_task<QString()> recognize_task([path = normalizedPhotoPath, face, storage]()
    {
        QString fresult;
        const QStringList mm = missingModules();

        if (mm.empty())
        {
            QTemporaryFile tmpFile;

            const QImage photo(path);
            const QImage face_photo = photo.copy(face);
            face_photo.save(&tmpFile, "JPEG");

            py::module find_faces = py::module::import("recognize_face");
            py::object result = find_faces.attr("recognize_face")(tmpFile.fileName().toStdString(),
                                                                  storage.toStdString());

            const std::string result_str = result.cast<py::str>();
            fresult = QString::fromStdString(result_str);
        }

        return fresult;
    });

    auto recognize_future = recognize_task.get_future();
    m_pythonThread->execute(recognize_task);

    recognize_future.wait();

    return recognize_future.get();
}


QString FaceRecognition::best(const QStringList& faces)
{
    std::packaged_task<QString()> optimize_task([faces]()
    {
        QString fresult;
        const QStringList mm = missingModules();

        if (mm.empty())
        {
            auto tmp_dir = System::getTmpDir("FaceRecognition_best");

            std::vector<std::string> face_files;
            face_files.reserve(faces.size());

            for (const QString& face: faces)
                face_files.push_back(face.toStdString());

            py::module find_faces = py::module::import("choose_best");
            py::object result = find_faces.attr("choose_best")(face_files,
                                                               tmp_dir->path().toStdString());

            const std::string result_str = result.cast<py::str>();
            fresult = QString::fromStdString(result_str);
        }

        return fresult;
    });

    auto optimize_future = optimize_task.get_future();
    m_pythonThread->execute(optimize_task);

    optimize_future.wait();

    return optimize_future.get();
}
