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

#include "face_recognition.hpp"

#include <cassert>
#include <future>
#include <memory>
#include <string>

#include <QByteArray>
#include <QDirIterator>
#include <QFileInfo>
#include <QImage>
#include <QRect>
#include <QString>
#include <QSaveFile>
#include <QTemporaryFile>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/image_tools.hpp>
#include <core/task_executor_utils.hpp>
#include <database/filter.hpp>
#include <database/ibackend.hpp>
#include <database/idatabase.hpp>
#include <system/filesystem.hpp>
#include <system/system.hpp>

#include "dlib_wrapper/dlib_face_recognition_api.hpp"


using namespace std::placeholders;

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
    std::mutex g_dlibMutex;   // global mutex for dlib usage.

    int chooseClosestMatching(const std::vector<double>& distances)
    {
        auto closest = std::min_element(distances.cbegin(), distances.cend());

        return (closest == distances.cend() || *closest > 0.6)? -1 : static_cast<int>(std::distance(distances.cbegin(), closest));
    }
}


struct FaceRecognition::Data
{
    explicit Data(ICoreFactoryAccessor* coreAccessor)
        : m_tmpDir(System::createTmpDir("FaceRecognition", System::Confidential))
        , m_logger(coreAccessor->getLoggerFactory()->get("FaceRecognition"))
        , m_exif(coreAccessor->getExifReaderFactory()->get())
    {

    }

    std::shared_ptr<ITmpDir> m_tmpDir;
    std::unique_ptr<ILogger> m_logger;
    IExifReader* m_exif;
};


FaceRecognition::FaceRecognition(ICoreFactoryAccessor* coreAccessor):
    m_data(std::make_unique<Data>(coreAccessor))
{

}


FaceRecognition::~FaceRecognition()
{

}


QVector<QRect> FaceRecognition::fetchFaces(const QString& path) const
{
    std::lock_guard lock(g_dlibMutex);
    QVector<QRect> result;

    OrientedImage orientedPhoto(m_data->m_exif, path);

    result = dlib_api::FaceLocator(m_data->m_logger.get()).face_locations(orientedPhoto.get(), 0);

    return result;
}


Person::Fingerprint FaceRecognition::getFingerprint(const OrientedImage& image, const QRect& face_rect)
{
    const QImage face = face_rect.isEmpty()? image.get(): image.get().copy(face_rect);

    dlib_api::FaceEncoder faceEndoder;
    const dlib_api::FaceEncodings face_encodings = faceEndoder.face_encodings(face);

    return face_encodings;
}


int FaceRecognition::recognize(const Person::Fingerprint& unknown, const std::vector<Person::Fingerprint>& known)
{
    const std::vector<double> distance = dlib_api::face_distance(known, unknown);
    const auto closestMatching = chooseClosestMatching(distance);

    return closestMatching;
}
