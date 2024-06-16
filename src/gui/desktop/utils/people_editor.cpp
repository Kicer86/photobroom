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

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <core/cast.hpp>
#include <core/containers_utils.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ilogger_factory.hpp>
#include <core/iexif_reader.hpp>
#include <core/task_executor_utils.hpp>
#include <database/ibackend.hpp>
#include <database/database_executor_traits.hpp>
#include <database/general_flags.hpp>
#include <face_recognition/face_recognition.hpp>

#include "implementation/people_editor_impl.hpp"

#include "people_editor.hpp"


using namespace Database::CommonGeneralFlags;
using namespace Qt::Literals::StringLiterals;


namespace
{
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
        return evaluate(db, [ph_id](Database::IBackend& backend)
        {
            const auto analysisState = backend.get(ph_id, FacesAnalysisState);

            return analysisState? *analysisState == static_cast<int>(FacesAnalysisType::AnalysedAndNotFound): false;
        });
    }

    QString pathFor(Database::IDatabase& db, const Photo::Id& id)
    {
        return evaluate(db, [id](Database::IBackend& backend)
        {
            auto photo = backend.getPhotoDelta(id, {Photo::Field::Path});

            return photo.get<Photo::Field::Path>();
        });
    }

    std::vector<QRect> detectFaces(const OrientedImage& image, const ILogger& logger)
    {
        return FaceRecognition(logger).fetchFaces(image);
    }

    void calculateMissingFingerprints(std::vector<PersonFullInfo>& faces, const OrientedImage& image, const ILogger& logger)
    {
        FaceRecognition face_recognition(logger);
        for (auto& faceInfo: faces)
            if (faceInfo.fingerprint.id().valid() == false)
            {
                const auto fingerprint = face_recognition.getFingerprint(image, faceInfo.position);

                faceInfo.fingerprint = PersonFingerprint(fingerprint);
            }
    }

    FaceEditor::PeopleData findPeople(const OrientedImage& image, const Photo::Id& id, const ILogger& logger)
    {
        FaceEditor::PeopleData data(id);

        // analyze photo - look for faces
        const auto detected_faces = detectFaces(image, logger);
        auto& people = data.get<Photo::Field::People>();
        std::ranges::transform(detected_faces, std::back_inserter(people), [](const QRect& rect)
        {
            PersonFullInfo pfi;
            pfi.position = rect;

            return pfi;
        });

        //calculate fingerprints
        calculateMissingFingerprints(people, image, logger);

        return data;
    }

    void sortFaces(std::vector<PersonFullInfo>& faces)
    {
        // sort faces so they appear from left to right
        std::sort(faces.begin(), faces.end(), [](const PersonFullInfo& lhs, const PersonFullInfo& rhs) {
            const auto& lhs_face = lhs.position;
            const auto& rhs_face = rhs.position;

            if (lhs_face.left() < rhs_face.left())        // lhs if left to rhs? - in order
                return true;
            else if (lhs_face.left() > rhs_face.right())  // lhs is right to rhs? - not in order
                return false;
            else
                return lhs_face.top() < rhs_face.top();   // when in line - lhs needs to be above
        });
    }
}

class Recognizer: public IRecognizePerson
{
    public:
        Recognizer(Database::IDatabase& db, const ILogger& logger)
            : m_logger(logger.subLogger("Recognizer"))
        {
            fetchPeopleAndFingerprints(db);
        }

        PersonName recognize(const PersonFullInfo& pi) override
        {
            FaceRecognition face_recognition(*m_logger);
            PersonName result;

            const std::vector<Person::Fingerprint>& known_fingerprints = std::get<0>(m_fingerprints);

            if (pi.name.name().isEmpty())
            {
                const int pos = face_recognition.recognize(pi.fingerprint.fingerprint(), known_fingerprints);

                if (pos >= 0)
                {
                    const std::vector<Person::Id>& known_people = std::get<1>(m_fingerprints);
                    const Person::Id found_person = known_people[safe_cast<std::size_t>(pos)];
                    result = m_people.at(found_person);
                }
            }

            return result;
        }

    private:
        using Fingerprints = std::tuple<std::vector<Person::Fingerprint>, std::vector<Person::Id>>;
        using People = std::map<Person::Id, PersonName>;
        Fingerprints m_fingerprints;
        People m_people;
        std::unique_ptr<ILogger> m_logger;

        void fetchPeopleAndFingerprints(Database::IDatabase& db)
        {
            evaluate(db, [this](Database::IBackend& backend)
            {
                std::vector<Person::Fingerprint> people_fingerprints;
                std::vector<Person::Id> people;

                const auto all_people = backend.peopleInformationAccessor().listPeople();

                for(const auto& person: all_people)
                {
                    m_people.emplace(person.id(), person);
                    const auto fingerprints = backend.peopleInformationAccessor().fingerprintsFor(person.id());

                    if (fingerprints.empty() == false)
                    {
                        people_fingerprints.push_back(average_fingerprint(fingerprints));
                        people.push_back(person.id());
                    }
                }

                m_fingerprints = std::tuple(people_fingerprints, people);
            });
        }
};


FaceEditor::FaceEditor(Database::IDatabase& db, ICoreFactoryAccessor& core, const ILogger& logger)
    : m_logger(logger.subLogger("FaceEditor"))
    , m_recognizer(std::make_shared<Recognizer>(db, *m_logger))
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

    auto faces = findFaces(*image, id);

    std::shared_ptr<Database::IClient> dbClient = m_db.attach(u"FaceEditor"_s);
    std::vector<std::unique_ptr<IFace>> result;

    std::ranges::transform(faces.get<Photo::Field::People>(), std::back_inserter(result), [id = faces.getId(), &image, &dbClient, recognizer = m_recognizer](const auto& personData)
    {
        return std::make_unique<Face>(id, personData, recognizer, image, dbClient);
    });

    return result;
}


FaceEditor::PeopleData FaceEditor::findFaces(const OrientedImage& image, const Photo::Id& id)
{
    FaceEditor::PeopleData result(id);

    const bool facesNotFound = wasPhotoAnalyzedAndHasNoFaces(m_db, id);

    // photo not analyzed yet (no records in db) or analyzed and we have data in db
    if (facesNotFound == false)
    {
        result = evaluate(m_db, [id](Database::IBackend& backend)
        {
            return backend.getPhotoDelta<Photo::Field::People>(id);
        });

        // no data in db
        if (result.get<Photo::Field::People>().empty())
        {
            result = findPeople(image, id, *m_logger);

            if (result.get<Photo::Field::People>().empty())
            {
                // mark photo as one without faces
                m_db.exec([id](Database::IBackend& backend)
                {
                    backend.set(
                        id,
                        FacesAnalysisState,
                        FacesAnalysisType::AnalysedAndNotFound);
                });
            }
            else
            {
                // store face location and fingerprint in db and update ids
                evaluate(m_db, [&result](Database::IBackend& backend)
                {
                    backend.update({result});

                    // refetch data to get updated ids
                    result = backend.getPhotoDelta<Photo::Field::People>(result.getId());
                });
            }
        }
    }

    sortFaces(result.get<Photo::Field::People>());

    return result;
}
