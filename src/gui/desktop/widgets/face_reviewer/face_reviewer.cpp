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
#include <QGroupBox>
#include <QScrollArea>
#include <QVBoxLayout>

#include <database/idatabase.hpp>

#include "face_gallery.hpp"
#include "utils/people_operator.hpp"
#include "project_utils/project.hpp"
#include "system/system.hpp"


using namespace std::placeholders;


FaceReviewer::FaceReviewer(Project* prj, ICoreFactoryAccessor* core, QWidget* p):
    QDialog(p),
    m_tmpDir(System::getTmpDir("FaceReviewer")),
    m_db(prj->getDatabase()),
    m_core(core),
    m_canvas(nullptr),
    m_project(prj)
{
    qRegisterMetaType<std::map<PersonName, std::vector<PersonInfo>>>("std::map<PersonName, std::vector<PersonInfo>>");
    qRegisterMetaType<std::map<Photo::Id, QString>>("std::map<Photo::Id, QString>");

    resize(640, 480);

    QVBoxLayout* l = new QVBoxLayout(this);
    QScrollArea* area = new QScrollArea(this);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    m_canvas = new QWidget(this);

    area->setWidgetResizable(true);
    area->setWidget(m_canvas);

    l->addWidget(area);
    l->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted,
            this, &QDialog::accept);

    connect(this, &FaceReviewer::gotPeopleInfo,
            this, &FaceReviewer::updatePeople);

    auto fetch = std::bind(&FaceReviewer::fetchPeople, this, _1);
    auto callback = m_safe_callback.make_safe_callback<void(Database::IBackendOperator *)>(fetch);

    m_db->performCustomAction(callback);
}


FaceReviewer::~FaceReviewer()
{

}


void FaceReviewer::fetchPeople(Database::IBackendOperator* op) const
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
            std::vector<PersonInfo> peopleInfo = op->listPeople(ph_id);

            // and find one for person we are insterested in
            for (const PersonInfo& pi: peopleInfo)
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

    emit gotPeopleInfo(details, photoToPath);
}


void FaceReviewer::updatePeople(const std::map<PersonName, std::vector<PersonInfo> >& details,
                                const std::map<Photo::Id, QString>& paths)
{
    PeopleOperator po(m_tmpDir->path(), m_db, m_core);
    QVBoxLayout* canvasLayout = new QVBoxLayout;

    for(const auto& detail: details)
    {
        QGroupBox* group = new QGroupBox(detail.first.name());
        canvasLayout->addWidget(group);

        const std::vector<PersonInfo>& peopleInfo = detail.second;

        std::vector<QImage> faceImages;
        faceImages.reserve(peopleInfo.size());

        QStringList faces;
        for(const PersonInfo& pi: peopleInfo)
        {
            auto it = paths.find(pi.ph_id);

            assert(it != paths.end());

            if (it != paths.end())
            {
                const QString& path = it->second;
                const QString absolute_path = m_project->makePathAbsolute(path);
                const QRect& faceRect = pi.rect;
                const QImage photo(absolute_path);
                const QImage face = photo.copy(faceRect);
                const QString file_path = System::getTmpFile(m_tmpDir->path(), "jpeg");

                face.save(file_path);

                faceImages.push_back(face);
                faces.append(file_path);
            }
        }

        QHBoxLayout* groupLayout = new QHBoxLayout(group);
        FaceGallery* gallery = new FaceGallery(group);

        groupLayout->addWidget(gallery);

        gallery->fill(faceImages);

        po.findBest(faces);
    }

    delete m_canvas->layout();
    m_canvas->setLayout(canvasLayout);
}
