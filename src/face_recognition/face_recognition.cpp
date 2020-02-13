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
#include <QImage>
#include <QFileInfo>
#include <QString>
#include <QRect>
#include <QSaveFile>
#include <QTemporaryFile>

#include <core/icore_factory_accessor.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/image_tools.hpp>
#include <database/filter.hpp>
#include <system/filesystem.hpp>
#include <system/system.hpp>

#include "dlib_wrapper/dlib_face_recognition_api.hpp"


using namespace std::placeholders;

namespace
{
    std::mutex g_dlibMutex;   // global mutex for dlib usage.

    dlib_api::FaceEncodings encodingsForFace(dlib_api::FaceEncoder& faceEndoder, const QString& face_image_path)
    {
        const QImage faceImage(face_image_path);
        return faceEndoder.face_encodings(faceImage);
    }


    dlib_api::FaceEncodings cachedEncodingForFace(dlib_api::FaceEncoder& faceEndoder, const QString& face_image_path)
    {
        dlib_api::FaceEncodings faceEncodings;

        const QFileInfo fileInfo(face_image_path);
        const QString fileName = fileInfo.baseName();
        const QString storage = fileInfo.absolutePath();
        const QString encodingsFilePath = storage + "/" + fileName + ".enc";

        if (QFile::exists(encodingsFilePath))
        {
            QFile encodingsFile(encodingsFilePath);
            encodingsFile.open(QFile::ReadOnly);

            while(encodingsFile.atEnd() == false)
            {
                const QByteArray line = encodingsFile.readLine();
                const double value = line.toDouble();

                faceEncodings.push_back(value);
            }
        }
        else
        {
            faceEncodings = encodingsForFace(faceEndoder, face_image_path);

            QSaveFile encodingsFile(encodingsFilePath);
            encodingsFile.open(QFile::WriteOnly);

            for(double v: faceEncodings)
            {
                const QByteArray line = QByteArray::number(v) + '\n';
                encodingsFile.write(line);
            }

            encodingsFile.commit();
        }

        return faceEncodings;
    }


    std::map<QString, dlib_api::FaceEncodings> encodingsForFaces(const QStringList& faces, dlib_api::FaceEncoder& faceEncoder)
    {
        std::map<QString, dlib_api::FaceEncodings> encoded_faces;
        for (const QString& face_path: faces)
        {
            const auto encoded_face = encodingsForFace(faceEncoder, face_path);
            encoded_faces[face_path] = encoded_face;
        }

        return encoded_faces;
    }

    std::tuple<std::vector<dlib_api::FaceEncodings>,
               std::vector<QString>
              >
    loadKnownFaces(const QString& knownFacesStorage, dlib_api::FaceEncoder& faceEndoder)
    {
        QDirIterator di(knownFacesStorage, { "*.jpg" });

        std::vector<dlib_api::FaceEncodings> known_faces;
        std::vector<QString> known_faces_names;

        while(di.hasNext())
        {
            const QString filePath = di.next();
            const QFileInfo fileInfo(filePath);

            const dlib_api::FaceEncodings faceEncodings = cachedEncodingForFace(faceEndoder, filePath);

            known_faces.push_back(faceEncodings);
            known_faces_names.push_back(fileInfo.fileName());
        }

        return { known_faces, known_faces_names };
    }

    std::vector<std::pair<double, QString>> zipNamesWithDistances(const std::vector<double>& distances, const std::vector<QString>& names)
    {
        assert(distances.size() == names.size());

        std::vector<std::pair<double, QString>> names_and_distances;
        std::size_t items = std::min(distances.size(), names.size());

        for(std::size_t i = 0; i < items; i++)
            names_and_distances.push_back( { distances[i], names[i] } );

        return names_and_distances;
    }


    void dropNotMatching(std::vector<std::pair<double, QString>>& names_and_distances)
    {
        names_and_distances.erase(std::remove_if(names_and_distances.begin(),
                                            names_and_distances.end(),
                                            [](const auto& nd) { return nd.first > 0.6; }),
                            names_and_distances.end());
    }


    QString chooseClosestMatching(const std::vector<double>& distances, const std::vector<QString>& names, ILogger& logger)
    {
        auto names_and_distances = zipNamesWithDistances(distances, names);
        dropNotMatching(names_and_distances);

        std::sort(names_and_distances.begin(), names_and_distances.end());

        // log best 3 results
        const auto items = names_and_distances.size();
        std::size_t to_log = std::min<std::size_t>(3, items);
        for(std::size_t i = 0; i < to_log; i++)
            logger.debug(QString("face %1 distance %2").arg(names_and_distances[i].second).arg(names_and_distances[i].first));

        logger.debug(QString("Found %1 face(s) with distance <= 0.6").arg(items));

        // return best matching
        return names_and_distances.empty()? QString() : names_and_distances.front().second;
    }


    QString faceToString(const QRect& face)
    {
        const auto string = QString("%1,%2 (%3x%4)")
                                .arg(face.left())
                                .arg(face.top())
                                .arg(face.width())
                                .arg(face.height());

        return string;
    }
}


struct FaceRecognition::Data
{
    Data(ICoreFactoryAccessor* coreAccessor)
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

    const QString normalizedPhotoPath = System::getTmpFile(m_data->m_tmpDir->path(), "jpeg");
    const bool s = Image::normalize(path, normalizedPhotoPath, m_data->m_exif);

    if (s)
    {
        QImage image(normalizedPhotoPath);

        result = dlib_api::FaceLocator(m_data->m_logger.get()).face_locations(image, 0);
    }

    return result;
}


QString FaceRecognition::recognize(const QString& path, const QRect& face, const QString& knownFacesStorage) const
{
    std::lock_guard lock(g_dlibMutex);
    dlib_api::FaceEncoder faceEndoder;

    QDirIterator di(knownFacesStorage, { "*.jpg" });

    auto [known_faces, known_faces_names] = loadKnownFaces(knownFacesStorage, faceEndoder);

    if (known_faces.empty())
        return {};
    else
    {
        const QString msg = QString("Trying to recognize face %1 from %2").arg(faceToString(face)).arg(path);
        m_data->m_logger->debug(msg);

        const QString normalizedPhotoPath = System::getTmpFile(m_data->m_tmpDir->path(), "jpeg");
        Image::normalize(path, normalizedPhotoPath, m_data->m_exif);

        const QImage photo(normalizedPhotoPath);
        const QImage face_photo = photo.copy(face);

        const dlib_api::FaceEncodings unknown_face_encodings = faceEndoder.face_encodings(face_photo);
        const std::vector<double> distance = dlib_api::face_distance(known_faces, unknown_face_encodings);

        const QString best_face_file = chooseClosestMatching(distance, known_faces_names, *m_data->m_logger.get());

        return best_face_file;
    }
}


QString FaceRecognition::best(const QStringList& faces)
{
    if (faces.size() < 3)           // we need at least 3 faces to do any serious job
    {
        m_data->m_logger->info("Not enought faces to find best one.");
        return {};
    }

    dlib_api::FaceEncoder faceEndoder;

    const std::map<QString, dlib_api::FaceEncodings> encoded_faces = encodingsForFaces(faces, faceEndoder);

    std::map<QString, double> average_distances;
    for(const auto& [face_path, face_encoding]: encoded_faces)
    {
        double total_distance = 0.0;
        int count = 0;

        for(const auto& [face_path2, face_encoding2]: encoded_faces)
        {
            if (face_path == face_path2)
                continue;

            const auto distance = dlib_api::face_distance({face_encoding}, face_encoding2);

            total_distance += distance.front();
            count++;
        }

        if (count > 0)
        {
            const double avg_distance = total_distance / count;
            average_distances[face_path] = avg_distance;
        }
    }

    for(auto& [path, avg_dist]: average_distances)
    {
        const QString msg = QString("Average distance for face %1 to other faces: %2").arg(path).arg(avg_dist);
        m_data->m_logger->debug(msg);
    }

    QString best_photo;
    if (average_distances.empty() == false)
    {
        const auto pos_of_best = std::min_element(average_distances.cbegin(), average_distances.cend(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.second < rhs.second;
        });

        best_photo = pos_of_best->first;
    }

    return best_photo;
}
