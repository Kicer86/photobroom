/*
 * Tool for people's face reviewing
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

#include "face_reviewer.hpp"

#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <database/idatabase.hpp>


FaceReviewer::FaceReviewer(Database::IDatabase* db, QWidget* p):
    QDialog(p),
    m_db(db)
{
    QVBoxLayout* l = new QVBoxLayout(this);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    l->addWidget(buttons);

    connect(this, &FaceReviewer::gotPeopleInfo,
            this, &FaceReviewer::updatePeople);

    m_db->performCustomAction([this](Database::IBackendOperator* op)
    {
        auto people = op->listPeople();

        std::map<PersonName, std::vector<PersonInfo>> details;
        std::map<Photo::Id, QString> photoToPath;
        std::set<Photo::Id> touchedPhotos;

        // for each person
        for(const auto& person: people)
        {
            // get photos it occurs on
            const Person::Id& p_id = person.id();
            auto filter = std::make_unique<Database::FilterPhotosWithPerson >(p_id);

            const std::vector<Photo::Id> matching_ids = op->getPhotos( { std::move(filter) } );

            // for each photo with given person
            for (const Photo::Id& ph_id: matching_ids)
            {
                // list detailed info about people
                std::vector<PersonInfo> people = op->listPeople(ph_id);

                // and find one for person we are insterested in
                for (const PersonInfo& pi: people)
                    if (pi.p_id == p_id)
                    {
                        details[person].push_back(pi);
                        touchedPhotos.insert(pi.ph_id);
                    }
            }
        }

        for(const Photo::Id& ph_id: touchedPhotos)
        {
            IPhotoInfo::Ptr photoInfo = op->getPhotoFor(ph_id);
            photoToPath[ph_id] = photoInfo->getPath();
        }
    });
}


FaceReviewer::~FaceReviewer()
{

}


void FaceReviewer::updatePeople(const std::map<PersonName, std::vector<PersonInfo> >& details,
                                const std::map<Photo::Id, QString>& paths)
{

}
