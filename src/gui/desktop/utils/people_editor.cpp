/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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

#include "people_editor.hpp"
#include "people_editor_impl_r++.hpp"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <core/containers_utils.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ilogger_factory.hpp>
#include <core/iexif_reader.hpp>
#include <core/json_serializer.hpp>
#include <core/task_executor_utils.hpp>
#include <database/ibackend.hpp>
#include <database/database_executor_traits.hpp>
#include <database/general_flags.hpp>
#include <face_recognition/face_recognition.hpp>

#include "implementation/faces_saver.hpp"
#include "implementation/people_editor_impl.hpp"


using namespace Database::CommonGeneralFlags;

namespace JSon
{
    template<typename T, typename Tag>
    struct CustomType<Id<T, Tag>>
    {
        using type = QJsonObject;

        static QJsonObject serialize(const Id<T, Tag>& id)
        {
            QJsonObject json;
            json["id"] = id.value();

            return json;
        }

        static Id<T, Tag> deserialize(const QJsonObject& json)
        {
            return Id<T, Tag>(json["id"].toVariant().value<T>());
        }
    };
}


namespace
{
    constexpr auto CacheBlob = "people_editor_cache";

    Person::Fingerprint average_fingerprint(const std::vector<PersonFingerprint>& faces)
    {
        if (faces.empty())
            return {};

        Person::Fingerprint avg_face;

        for(const PersonFingerprint& fingerprint: faces)
            avg_face += fingerprint.fingerprint();

        avg_face /= static_cast<double>(faces.size());

        return avg_face;
    }

    bool wasPhotoAnalyzedAndHasNoFaces(Database::IDatabase& db, const Photo::Id& ph_id)
    {
        return evaluate<bool(Database::IBackend &)>(db, [ph_id](Database::IBackend& backend)
        {
            const auto analysisState = backend.get(ph_id, FacesAnalysisState);

            return analysisState? *analysisState == static_cast<int>(FacesAnalysisType::AnalysedAndNotFound): false;
        });
    }

    QString pathFor(Database::IDatabase& db, const Photo::Id& id)
    {
        return evaluate<QString(Database::IBackend &)>(db, [id](Database::IBackend& backend)
        {
            auto photo = backend.getPhotoDelta(id, {Photo::Field::Path});

            return photo.get<Photo::Field::Path>();
        });
    }

    std::vector<PersonInfo> fetchFacesFromDb(Database::IDatabase& db, const Photo::Id& ph_id)
    {
        return evaluate<std::vector<PersonInfo>(Database::IBackend &)>(db, [ph_id](Database::IBackend& backend)
        {
            std::vector<PersonInfo> faces;

            const auto people = backend.peopleInformationAccessor().listPeople(ph_id);
            std::ranges::copy_if(people, std::back_inserter(faces), [](const PersonInfo& pi) { return pi.rect.isValid(); });

            return faces;
        });
    }

    PersonName personName(Database::IDatabase& db, const Person::Id& id)
    {
        const PersonName person = evaluate<PersonName (Database::IBackend &)>
            (db, [id](Database::IBackend& backend)
        {
            const auto people = backend.peopleInformationAccessor().person(id);

            return people;
        });

        return person;
    }

    std::tuple<std::vector<Person::Fingerprint>, std::vector<Person::Id>> fetchPeopleAndFingerprints(Database::IDatabase& db)
    {
        typedef std::tuple<std::vector<Person::Fingerprint>, std::vector<Person::Id>> Result;

        return evaluate<Result(Database::IBackend &)>(db, [](Database::IBackend& backend)
        {
            std::vector<Person::Fingerprint> people_fingerprints;
            std::vector<Person::Id> people;

            const auto all_people = backend.peopleInformationAccessor().listPeople();
            for(const auto& person: all_people)
            {
                const auto fingerprints = backend.peopleInformationAccessor().fingerprintsFor(person.id());

                if (fingerprints.empty() == false)
                {
                    people_fingerprints.push_back(average_fingerprint(fingerprints));
                    people.push_back(person.id());
                }
            }

            return std::tuple(people_fingerprints, people);
        });
    }

    std::vector<QRect> detectFaces(const OrientedImage& image, const ILogger& logger)
    {
        return FaceRecognition(logger).fetchFaces(image);
    }

    void calculateMissingFingerprints(std::vector<FaceInfo>& faces, const OrientedImage& image, const ILogger& logger)
    {
        FaceRecognition face_recognition(logger);
        for (FaceInfo& faceInfo: faces)
            if (faceInfo.fingerprint.id().valid() == false)
            {
                const auto fingerprint = face_recognition.getFingerprint(image, faceInfo.face.rect);

                faceInfo.fingerprint = PersonFingerprint(fingerprint);
            }
    }

    void recognizePeople(std::vector<FaceInfo>& faces, Database::IDatabase& db, const ILogger& logger)
    {
        FaceRecognition face_recognition(logger);
        const auto people_fingerprints = fetchPeopleAndFingerprints(db);
        const std::vector<Person::Fingerprint>& known_fingerprints = std::get<0>(people_fingerprints);

        for (FaceInfo& faceInfo: faces)
            if (faceInfo.person.name().isEmpty())
            {
                const int pos = face_recognition.recognize(faceInfo.fingerprint.fingerprint(), known_fingerprints);

                if (pos >= 0)
                {
                    const std::vector<Person::Id>& known_people = std::get<1>(people_fingerprints);
                    const Person::Id found_person = known_people[pos];
                    faceInfo.person = personName(db, found_person);
                }
            }
    }

    std::vector<FaceInfo> findAndRecognizePeople(const OrientedImage& image, Database::IDatabase& db, const Photo::Id& id, const ILogger& logger)
    {
        std::vector<FaceInfo> result;

        // analyze photo - look for faces
        const auto detected_faces = detectFaces(image, logger);
        std::ranges::transform(detected_faces, std::back_inserter(result), [id](const QRect& rect)
        {
            return FaceInfo(id, rect);
        });

        //calculate fingerprints
        calculateMissingFingerprints(result, image, logger);

        //recognize people
        recognizePeople(result, db, logger);

        return result;
    }

    void sortFaces(std::vector<FaceInfo>& faces)
    {
        // sort faces so they appear from left to right
        std::sort(faces.begin(), faces.end(), [](const FaceInfo& lhs, const FaceInfo& rhs) {
            const auto lhs_face = lhs.face.rect;
            const auto rhs_face = rhs.face.rect;

            if (lhs_face.left() < rhs_face.left())        // lhs if left to rhs? - in order
                return true;
            else if (lhs_face.left() > rhs_face.right())  // lhs is right to rhs? - not in order
                return false;
            else
                return lhs_face.top() < rhs_face.top();   // when in line - lhs needs to be above
        });
    }

    std::vector<FaceInfo> loadCachedFacesFor(Database::IDatabase& db, const Photo::Id& id)
    {
        std::vector<FaceInfo> result;

        const QByteArray blob = evaluate<QByteArray(Database::IBackend &)>(db, [id](Database::IBackend& backend)
        {
            return backend.readBlob(id, CacheBlob);
        });

        if (blob.isEmpty() == false)
        {
            const QJsonDocument json = QJsonDocument::fromJson(blob);
            const std::vector<CalculatedData> storage = JSon::deserialize<std::vector<CalculatedData>>(json);

            for (const auto& faceData: storage)
            {
                FaceInfo fi(faceData.ph_id, faceData.position);
                fi.fingerprint = PersonFingerprint(faceData.fingerprint);
                fi.person = PersonName(faceData.name);

                result.push_back(fi);
            }
        }

        return result;
    }

    void cacheFacesFor(Database::IDatabase& db, const Photo::Id& id, const std::vector<FaceInfo>& result)
    {
        std::vector<CalculatedData> storage;

        for (const auto& faceInfo: result)
            storage.emplace_back(faceInfo.face.rect, faceInfo.fingerprint.fingerprint(), faceInfo.person.name(), faceInfo.face.ph_id);

        // store detected, but not confirmed by user, faces as blob in database for cache
        const QJsonDocument json = JSon::serialize(storage);

        execute(db, [id, blob = json.toJson()](Database::IBackend& backend)
        {
            backend.writeBlob(id, CacheBlob, blob);
        });
    }

    std::vector<FaceInfo> findFaces(Database::IDatabase& db,
                                    const OrientedImage& image,
                                    const ILogger& logger,
                                    const Photo::Id& id)
    {
        std::vector<FaceInfo> result;

        const bool facesNotFound = wasPhotoAnalyzedAndHasNoFaces(db, id);

        // photo not analyzed yet (no records in db) or analyzed and we have data in db
        if (facesNotFound == false)
        {
            const std::vector<PersonInfo> list_of_faces = fetchFacesFromDb(db, id);

            // no data in db
            if (list_of_faces.empty())
            {
                // check in cache
                result = loadCachedFacesFor(db, id);

                if (result.empty())
                {
                    result = findAndRecognizePeople(image, db, id, logger);

                    if (result.empty())
                    {
                        // mark photo as one without faces
                        db.exec([id](Database::IBackend& backend)
                        {
                            backend.set(
                                id,
                                FacesAnalysisState,
                                FacesAnalysisType::AnalysedAndNotFound);
                        });
                    }
                    else
                        cacheFacesFor(db, id, result);
                }
            }
            else    // data in db just use it
            {
                evaluate<void(Database::IBackend &)>(db, [&result, &list_of_faces](Database::IBackend& backend)
                {
                    std::vector<PersonInfo::Id> pi_ids;
                    std::ranges::transform(list_of_faces, std::back_inserter(pi_ids), [](const PersonInfo& pi)
                    {
                        return pi.id;
                    });

                    const auto fingerprints = backend.peopleInformationAccessor().fingerprintsFor(pi_ids);
                    assert(fingerprints.size() == list_of_faces.size());

                    for (const PersonInfo& pi: list_of_faces)
                    {
                        FaceInfo fi(pi);
                        fi.fingerprint = fingerprints.find(pi.id)->second;

                        if (pi.p_id.valid())
                            fi.person = backend.peopleInformationAccessor().person(pi.p_id);

                        result.push_back(fi);
                    };
                });
            }
        }

        sortFaces(result);

        return result;
    }
}


FaceEditor::FaceEditor(Database::IDatabase& db, ICoreFactoryAccessor& core, const ILogger& logger)
    : m_logger(logger.subLogger("FaceEditor"))
    , m_db(db)
    , m_core(core)
{

}


std::vector<std::unique_ptr<IFace>> FaceEditor::getFacesFor(const Photo::Id& id)
{
    const QString path = pathFor(m_db, id);
    const QFileInfo pathInfo(path);
    const QString full_path = pathInfo.absoluteFilePath();
    auto image = std::make_shared<OrientedImage>(m_core.getExifReaderFactory().get(), full_path);

    const auto faces = findFaces(
        m_db,
        *image,
        *m_logger,
        id);

    auto storage = m_facesSaver.lock();
    if (storage.get() == nullptr)
    {
        storage = std::make_shared<FacesSaver>(m_db);
        m_facesSaver = storage;
    }

    std::vector<std::unique_ptr<IFace>> result;

    std::ranges::transform(faces, std::back_inserter(result), [&image, &storage](const FaceInfo& fi)
    {
        return std::make_unique<Face>(fi, image, storage);
    });

    return result;
}
