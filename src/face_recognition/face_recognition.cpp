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
#include <memory>
#include <string>

#include <QByteArray>
#include <QDirIterator>
#include <QElapsedTimer>
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

#include "dlib_wrapper/dlib_face_recognition_api.hpp"
#include "unit_tests_utils/empty_logger.hpp"


import system;


using namespace std::placeholders;

namespace
{
    std::mutex g_dlibMutex;   // global mutex for dlib usage.

    int chooseClosestMatching(const std::vector<double>& distances)
    {
        auto closest = std::min_element(distances.cbegin(), distances.cend());

        return (closest == distances.cend() || *closest > 0.6)? -1 : static_cast<int>(std::distance(distances.cbegin(), closest));
    }
}



FaceRecognition::FaceRecognition(const ILogger& logger)
    : m_logger(logger.subLogger("FaceRecognition"))
{

}


FaceRecognition::~FaceRecognition()
{

}


bool FaceRecognition::checkSystem()
{
    return dlib_api::check_system_prerequisites();
}


std::vector<QRect> FaceRecognition::fetchFaces(const OrientedImage& orientedPhoto) const
{
    const int pixels = orientedPhoto->width() * orientedPhoto->height();
    const double mpixels = pixels / 1e6;

    m_logger->debug(QString("Looking for faces in photo of size: %2Mpx")
        .arg(mpixels, 0, 'f', 1)
    );

    QElapsedTimer timer;
    timer.start();

    const auto faces = fetchFaces(orientedPhoto, 1);
    const auto elapsed = timer.elapsed();

    m_logger->info(QString("Found %1 faces in time: %2ms")
        .arg(faces.size())
        .arg(elapsed)
    );

    return std::vector<QRect>(faces.begin(), faces.end());
}


Person::Fingerprint FaceRecognition::getFingerprint(const OrientedImage& image, const QRect& face_rect)
{
    const QImage face = face_rect.isEmpty()? image.get(): image.get().copy(face_rect);

    dlib_api::FaceEncoder faceEndoder(m_logger.get());
    const dlib_api::FaceEncodings face_encodings = faceEndoder.face_encodings(face);

    return face_encodings;
}


int FaceRecognition::recognize(const Person::Fingerprint& unknown, const std::vector<Person::Fingerprint>& known)
{
    const std::vector<double> distance = dlib_api::face_distance(known, unknown);
    const auto closestMatching = chooseClosestMatching(distance);

    return closestMatching;
}


QVector<QRect> FaceRecognition::fetchFaces(const OrientedImage& orientedPhoto, double scale) const
{
    std::lock_guard<std::mutex> lock(g_dlibMutex);
    QVector<QRect> result;

    const QSize scaledSize = orientedPhoto.get().size() * scale;
    const QImage photo = orientedPhoto.get().scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    result = dlib_api::FaceLocator(m_logger.get()).face_locations(photo, 0);

    std::transform(result.begin(), result.end(), result.begin(), [scale](const QRect& face){
        return QRectF(face.topLeft().x() / scale, face.topLeft().y() / scale,
                      face.width() / scale, face.height() / scale).toRect();
    });

    return result;
}
