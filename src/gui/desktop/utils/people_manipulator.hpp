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

#ifndef PEOPLEMANIPULATOR_HPP
#define PEOPLEMANIPULATOR_HPP

#include <core/function_wrappers.hpp>
#include <core/oriented_image.hpp>
#include <database/photo_types.hpp>
#include <database/idatabase.hpp>


struct ICoreFactoryAccessor;


class PeopleManipulator: public QObject
{
        Q_OBJECT

    public:
        PeopleManipulator(const Photo::Id &, Database::IDatabase &, ICoreFactoryAccessor &);
        ~PeopleManipulator();

        std::size_t facesCount() const;
        const QString& name(std::size_t) const;
        const QRect& position(std::size_t) const;
        QSize photoSize() const;

        void setName(std::size_t, const QString &);
        void store();

    signals:
        void facesAnalyzed() const;

    private:
        struct FaceInfo
        {
            PersonInfo face;
            PersonName person;
            PersonFingerprint fingerprint;

            FaceInfo(const QRect& r)
            {
                face.rect = r;
            }
        };

        safe_callback_ctrl m_callback_ctrl;
        std::vector<FaceInfo> m_faces;
        OrientedImage m_image;
        Photo::Id m_pid;
        ICoreFactoryAccessor& m_core;
        Database::IDatabase& m_db;

        void runOnThread(void (PeopleManipulator::*)());

        void findFaces();
        void findFaces_thrd();
        void findFaces_result(const QVector<QRect> &);

        void recognizeFaces();
        void recognizeFaces_thrd_fetch_from_db();
        void recognizeFaces_thrd_calculate_missing_fingerprints();
        void recognizeFaces_thrd_recognize_people();
        void recognizeFaces_thrd();
        void recognizeFaces_result();

        void store_people_names();
        void store_fingerprints();
        void store_people_information();

        std::vector<QRect> fetchFacesFromDb() const;
        std::vector<PersonInfo> fetchPeopleFromDb() const;
        std::tuple<std::vector<Person::Fingerprint>, std::vector<Person::Id>> fetchPeopleAndFingerprints() const;
        std::map<PersonInfo::Id, PersonFingerprint> fetchFingerprints(const std::vector<PersonInfo::Id>& ids) const;
        std::vector<PersonName> fetchPeople() const;
        PersonName personData(const Person::Id& id) const;
        PersonName storeNewPerson(const QString& name) const;
        QString pathFor(const Photo::Id& id) const;
};

#endif // PEOPLEMANIPULATOR_HPP
