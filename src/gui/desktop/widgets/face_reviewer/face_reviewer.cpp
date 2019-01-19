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

#include <core/icore_factory_accessor.hpp>
#include <core/itask_executor.hpp>
#include <core/task_executor_utils.hpp>
#include <database/idatabase.hpp>

#include "face_details.hpp"
#include "project_utils/project.hpp"
#include "system/system.hpp"


using namespace std::placeholders;


FaceReviewer::FaceReviewer(Project* prj, ICoreFactoryAccessor* core, QWidget* p):
    QDialog(p),
    m_operator(prj->getProjectInfo().getInternalLocation(ProjectInfo::FaceRecognition),
               prj->getDatabase(),
               core),
    m_optimizer(core, &m_operator),
    m_tmpDir(System::getTmpDir("FaceReviewer")),
    m_db(prj->getDatabase()),
    m_core(core),
    m_canvas(nullptr),
    m_project(prj)
{
    qRegisterMetaType<std::map<PersonName, std::vector<PersonInfo>>>("std::map<PersonName, std::vector<PersonInfo>>");
    qRegisterMetaType<std::map<Photo::Id, QString>>("std::map<Photo::Id, QString>");
    qRegisterMetaType<PersonInfo>("PersonInfo");

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
    auto callback = m_safe_callback.make_safe_callback<void(Database::IBackend *)>(fetch);

    m_db->exec(callback);
}


FaceReviewer::~FaceReviewer()
{
    m_safe_callback.invalidate();
}


void FaceReviewer::fetchPeople(Database::IBackend* op) const
{
    auto people = op->listPeople();

    std::map<PersonName, std::vector<PersonInfo>> details;

    // for each person
    for(const auto& person: people)
    {
        // get photos it occurs on
        const Person::Id& p_id = person.id();
        auto filter = std::make_unique<Database::FilterPhotosWithPerson>(p_id);

        const std::vector<Photo::Id> matching_ids = op->getPhotos( { std::move(filter) } );

        // for each photo with given person
        for (const Photo::Id& ph_id: matching_ids)
        {
            // list detailed info about people
            std::vector<PersonInfo> peopleInfo = op->listPeople(ph_id);

            // and find one for person we are insterested in
            for (const PersonInfo& pi: peopleInfo)
                if (pi.p_id == p_id)
                    details[person].push_back(pi);
        }
    }

    emit gotPeopleInfo(details);
}


void FaceReviewer::updatePeople(const std::map<PersonName, std::vector<PersonInfo> >& details)
{
    m_infos.clear();

    for(const auto& detail: details)
        m_infos[detail.first.id()] = detail.second;

    QVBoxLayout* canvasLayout = new QVBoxLayout;

    for(const auto& detail: details)
    {
        const PersonName& p_name = detail.first;
        const std::vector<PersonInfo>& peopleInfo = detail.second;

        FaceDetails* faceDetails = new FaceDetails(p_name.name(),
                                                   &m_optimizer,
                                                   m_core->getTaskExecutor(),
                                                   &m_operator,
                                                   peopleInfo,
                                                   this);
        canvasLayout->addWidget(faceDetails);
    }

    assert(m_canvas->layout() == nullptr);
    m_canvas->setLayout(canvasLayout);
}
