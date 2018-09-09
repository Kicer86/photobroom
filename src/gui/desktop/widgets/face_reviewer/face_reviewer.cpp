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
#include "project_utils/project.hpp"
#include "system/system.hpp"


using namespace std::placeholders;


FaceReviewer::FaceReviewer(Project* prj, ICoreFactoryAccessor* core, QWidget* p):
    QDialog(p),
    m_operator(prj->getProjectInfo().getInternalLocation(ProjectInfo::FaceRecognition),
               prj->getDatabase(),
               core),
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
            &m_operator,  &PeopleOperator::setModelFace);

    connect(&m_optimizer, &FaceOptimizer::error,
            this,         &FaceReviewer::updatePersonFailed);

    connect(&m_operator, &PeopleOperator::modelFaceSet,
            this,        qOverload<const Person::Id &>(&FaceReviewer::updatePerson));

    auto fetch = std::bind(&FaceReviewer::fetchPeople, this, _1);
    auto callback = m_safe_callback.make_safe_callback<void(Database::IBackendOperator *)>(fetch);

    m_db->performCustomAction(callback);
}


FaceReviewer::~FaceReviewer()
{
    m_safe_callback.invalidate();
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
    m_infos.clear();

    for(const auto& detail: details)
        m_infos[detail.first.id()] = detail.second;

    QVBoxLayout* canvasLayout = new QVBoxLayout;

    for(const auto& detail: details)
    {
        FaceDetails* faceDetails = new FaceDetails(detail.first.name(), this);
        canvasLayout->addWidget(faceDetails);

        const PersonName& p_name = detail.first;
        const std::vector<PersonInfo>& peopleInfo = detail.second;

        faceDetails->setOccurrences(peopleInfo.size());
        updatePerson(faceDetails, p_name.id());

        connect(faceDetails, &FaceDetails::optimize, std::bind(&FaceReviewer::optimize, this, p_name.id()));

        m_faceWidgets.emplace(p_name.id(), faceDetails);
    }

    delete m_canvas->layout();
    m_canvas->setLayout(canvasLayout);
}


void FaceReviewer::updatePerson(const Person::Id& id)
{
    FaceDetails* fd = detailsFor(id);

    fd->enableOptimizationButton(true);

    updatePerson(fd, id);
}


void FaceReviewer::updatePerson(FaceDetails* group, const Person::Id& id)
{
    const QString modelFacePath = m_operator.getModelFace(id);

    // set pixmap with face.
    // As pixmap preparations may be heavy, perform them in a thread
    if (modelFacePath.isEmpty() == false)
    {
        auto* taskMgr = m_core->getTaskExecutor();

        runOn(taskMgr, [group, modelFacePath]
        {
            const QImage faceImg(modelFacePath);
            const QImage scaled = faceImg.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            // do not call slot directly - make sure it will be called from main thread
            QMetaObject::invokeMethod(group, "setModelPhoto", Q_ARG(QImage, scaled));
        });
    }
}


void FaceReviewer::updatePersonFailed(const Person::Id& id) const
{
    FaceDetails* fd = detailsFor(id);
    fd->enableOptimizationButton(true);
}


void FaceReviewer::optimize(const Person::Id& id)
{
    FaceDetails* fd = detailsFor(id);
    fd->enableOptimizationButton(false);

    auto it = m_infos.find(id);
    assert(it != m_infos.end());

    const std::vector<PersonInfo>& peopleInfo = it->second;

    m_optimizer.optimize(id, peopleInfo, m_paths);
}


FaceDetails* FaceReviewer::detailsFor(const Person::Id& id) const
{
    auto it = m_faceWidgets.find(id);

    assert(it != m_faceWidgets.cend());

    return it->second;
}


///////////////////////////////////////////////////////////////////////////////


FaceOptimizer::FaceOptimizer(Database::IDatabase* db,
                             ICoreFactoryAccessor* core):
    m_tmpDir(System::getTmpDir("FaceOptimizer")),
    m_db(db),
    m_core(core)
{
}


FaceOptimizer::~FaceOptimizer()
{
    m_safe_callback.invalidate();
}


void FaceOptimizer::optimize(const Person::Id& pid,
                             const std::vector<PersonInfo>& pis,
                             const std::map<Photo::Id, QString>& paths)
{
    auto task = [this, pid, pis, paths]
    {
        FaceRecognition face_recognition(m_core);

        const auto path2Person = saveFiles(pis, paths);

        QStringList files;
        std::copy(key_map_iterator<decltype(path2Person)>(path2Person.cbegin()),
                  key_map_iterator<decltype(path2Person)>(path2Person.cend()),
                  std::back_inserter(files));

        const auto best_face_path = face_recognition.best(files);

        auto it = path2Person.find(best_face_path);

        assert(it != path2Person.cend() || best_face_path.isEmpty()); // if `best_face_path` isn't empty, then we should find person

        if (it == path2Person.cend())
            emit error(pid);
        else
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
