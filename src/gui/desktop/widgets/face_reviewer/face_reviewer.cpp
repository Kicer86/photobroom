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
#include <core/map_iterator.hpp>
#include <core/task_executor_utils.hpp>
#include <database/idatabase.hpp>
#include <face_recognition/face_recognition.hpp>

#include "face_details.hpp"
#include "utils/people_operator.hpp"
#include "project_utils/project.hpp"
#include "system/system.hpp"


using namespace std::placeholders;


FaceReviewer::FaceReviewer(Project* prj, ICoreFactoryAccessor* core, QWidget* p):
    QDialog(p),
    m_optimizer(prj->getDatabase(), core),
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

    connect(&m_optimizer, &FaceOptimizer::best,
            this,         &FaceReviewer::setBest);

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
    m_paths = paths;
    m_details.clear();

    for(const auto& detail: details)
        m_details[detail.first.id()] = detail.second;

    const QString faceDataDir = m_project->getProjectInfo().getInternalLocation(ProjectInfo::FaceRecognition);
    PeopleOperator po(faceDataDir, m_db, m_core);
    QVBoxLayout* canvasLayout = new QVBoxLayout;

    auto* taskMgr = m_core->getTaskExecutor();

    for(const auto& detail: details)
    {
        FaceDetails* group = new FaceDetails(detail.first.name(), this);
        canvasLayout->addWidget(group);

        const PersonName& p_name = detail.first;
        const std::vector<PersonInfo>& peopleInfo = detail.second;

        group->setOccurrences(peopleInfo.size());

        const QString modelFacePath = po.getModelFace(p_name.id());

        // set pixmap with face.
        // As pixmap preparations may be heavy, perform them in a thread
        if (modelFacePath.isEmpty() == false)
            runOn(taskMgr, [group, modelFacePath]
            {
                const QImage faceImg(modelFacePath);
                const QImage scaled = faceImg.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

                // do not call slot directly - make sure it will be called from main thread
                QMetaObject::invokeMethod(group, "setModelPhoto", Q_ARG(QImage, scaled));
            });

        connect(group, &FaceDetails::optimize, std::bind(&FaceReviewer::optimize, this, p_name.id()));
    }

    delete m_canvas->layout();
    m_canvas->setLayout(canvasLayout);
}


void FaceReviewer::optimize(const Person::Id& id)
{
    auto it = m_details.find(id);
    assert(it != m_details.end());

    const std::vector<PersonInfo>& peopleInfo = it->second;

    m_optimizer.optimize(peopleInfo, m_paths);
}


void FaceReviewer::setBest(const PersonInfo& pi)
{
    const QString faceDataDir = m_project->getProjectInfo().getInternalLocation(ProjectInfo::FaceRecognition);
    PeopleOperator po(faceDataDir, m_db, m_core);

    po.setModelFace(pi);
}


///////////////////////////////////////////////////////////////////////////////


FaceOptimizer::FaceOptimizer(Database::IDatabase* db,
                             ICoreFactoryAccessor* core):
    m_tmpDir(System::getTmpDir("FaceOptimizer")),
    m_db(db),
    m_core(core)
{
}


void FaceOptimizer::optimize(const std::vector<PersonInfo>& pis,
                             const std::map<Photo::Id, QString>& paths)
{
    auto task = [this, pis, paths]
    {
        FaceRecognition face_recognition(m_core);

        const auto path2Person = saveFiles(pis, paths);

        QStringList files;
        std::copy(key_map_iterator<decltype(path2Person)>(path2Person.cbegin()),
                  key_map_iterator<decltype(path2Person)>(path2Person.cend()),
                  std::back_inserter(files));

        const auto best_face_path = face_recognition.best(files);

        auto it = path2Person.find(best_face_path);
        assert(it != path2Person.cend());

        if (it != path2Person.cend())
            emit best(it->second);
    };

    auto safe_task = m_safe_callback.make_safe_callback<void()>(task);
    auto* taskMgr = m_core->getTaskExecutor();

    runOn(taskMgr, safe_task);
}


std::map<QString, PersonInfo> FaceOptimizer::saveFiles(const std::vector<PersonInfo>& pis,
                                                       const std::map<Photo::Id, QString>& paths)
{
    std::map<QString, PersonInfo> results;
    for(const PersonInfo& pi: pis)
    {
        auto it = paths.find(pi.ph_id);

        assert(it != paths.end());

        if (it != paths.end())
        {
            const QString& path = it->second;
            const QRect& faceRect = pi.rect;
            const QImage photo(path);
            const QImage face = photo.copy(faceRect);
            const QString file_path = System::getTmpFile(m_tmpDir->path(), "jpeg");

            face.save(file_path);

            results.emplace(file_path, pi);
        }
    }

    return results;
}
