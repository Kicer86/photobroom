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


    dlib_api::FaceEncodings encodingsForFace(dlib_api::FaceEncoder& faceEndoder, const QString& face_image_path)
    {
        const QImage faceImage(face_image_path);
        return faceEndoder.face_encodings(faceImage);
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


    auto fetchPeopleAndEncodings(Database::IBackend& backend)
    {
        std::vector<dlib_api::FaceEncodings> encodings;
        std::vector<Person::Id> people;

        const auto all_people = backend.peopleInformationAccessor().listPeople();
        for(const auto& person: all_people)
        {
            const auto fingerprints = backend.peopleInformationAccessor().fingerprintsFor(person.id());

            if (fingerprints.empty() == false)
            {
                encodings.push_back(fingerprints.front().fingerprint());
                people.push_back(person.id());
            }
        }

        return std::tuple(encodings, people);
    }


    std::vector<std::pair<double, Person::Id>> zipNamesWithDistances(const std::vector<double>& distances, const std::vector<Person::Id>& names)
    {
        assert(distances.size() == names.size());

        std::vector<std::pair<double, Person::Id>> names_and_distances;
        std::size_t items = std::min(distances.size(), names.size());

        for(std::size_t i = 0; i < items; i++)
            names_and_distances.push_back( { distances[i], names[i] } );

        return names_and_distances;
    }


    void dropNotMatching(std::vector<std::pair<double, Person::Id>>& names_and_distances)
    {
        names_and_distances.erase(std::remove_if(names_and_distances.begin(),
                                            names_and_distances.end(),
                                            [](const auto& nd) { return nd.first > 0.6; }),
                            names_and_distances.end());
    }


    Person::Id chooseClosestMatching(const std::vector<double>& distances, const std::vector<Person::Id>& names, ILogger& logger)
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
        return names_and_distances.empty()? Person::Id() : names_and_distances.front().second;
    }


    int chooseClosestMatching(const std::vector<double>& distances)
    {
        auto closest = std::min_element(distances.cbegin(), distances.cend());

        return (closest == distances.cend() || *closest > 0.6)? -1 : static_cast<int>(std::distance(distances.cbegin(), closest));
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

    const QString normalizedPhotoPath = System::getTmpFile(m_data->m_tmpDir->path(), "jpeg");
    const bool s = Image::normalize(path, normalizedPhotoPath, m_data->m_exif);

    if (s)
    {
        QImage image(normalizedPhotoPath);

        result = dlib_api::FaceLocator(m_data->m_logger.get()).face_locations(image, 0);
    }

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
