
#include <ranges>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

#include <core/media_types.hpp>
#include <core/task_executor_utils.hpp>
#include <core/ilogger_factory.hpp>
#include <database/database_executor_traits.hpp>
#include <database/igroup_operator.hpp>
#include <database/photo_utils.hpp>
#include <face_recognition/face_recognition.hpp>
#include "ui/faces_dialog.hpp"
#include "ui/photos_grouping_dialog.hpp"
#include "utils/groups_manager.hpp"
#include "context_menu_manager.hpp"


ContextMenuManager::ContextMenuManager()
    : m_enableFaceRecognition(FaceRecognition::checkSystem())
{

}


const QAbstractItemModel* ContextMenuManager::model() const
{
    return &m_model;
}


QList<QVariant> ContextMenuManager::selection() const
{
    return m_selection;
}


Project* ContextMenuManager::project() const
{
    return m_project;
}


ICoreFactoryAccessor * ContextMenuManager::coreFactory() const
{
    return m_core;
}


void ContextMenuManager::setSelection(const QList<QVariant>& selection)
{
    m_selection = selection;

    const auto selectedIds = std::ranges::views::transform(selection, [](const QVariant& item)
    {
        return item.value<Photo::Id>();
    });

    if (m_translator)
        m_translator->selectedPhotos({selectedIds.begin(), selectedIds.end()});

    emit selectionChanged(m_selection);
}


void ContextMenuManager::setProject(Project* prj)
{
    m_project = prj;
    m_translator.reset();

    if (m_project)
    {
        m_translator = std::make_unique<IdToDataConverter>(m_project->getDatabase());
        connect(m_translator.get(), &IdToDataConverter::selectionChanged,
                this, &ContextMenuManager::updateModel);
    }
}


void ContextMenuManager::setCoreFactory(ICoreFactoryAccessor* core)
{
    m_core = core;

    if (m_enableFaceRecognition == false)
        m_core->getLoggerFactory().get("ContextMenuManager")->warning("Face recognition cannot be enabled");
}


void ContextMenuManager::updateModel(const std::vector<Photo::Data>& selectedPhotos)
{
    m_photos.clear();
    m_model.clear();

    std::remove_copy_if(selectedPhotos.cbegin(), selectedPhotos.cend(), std::back_inserter(m_photos), [](const Photo::Data& photo)
    {
        return QFile::exists(photo.path) == false;
    });

    if (m_photos.empty())
        return;

    QAction* groupPhotos    = new QAction(tr("Group"));
    QAction* manageGroup    = new QAction(tr("Manage group..."));
    QAction* ungroupPhotos  = new QAction(tr("Ungroup"));
    QAction* location       = new QAction(tr("Open photo location"));
    QAction* faces          = new QAction(tr("Recognize people..."));

    connect(groupPhotos, &QAction::triggered, this, &ContextMenuManager::groupPhotosAction);
    connect(manageGroup, &QAction::triggered, this, &ContextMenuManager::manageGroupsAction);
    connect(ungroupPhotos, &QAction::triggered, this, &ContextMenuManager::ungroupAction);
    connect(location,    &QAction::triggered, this, &ContextMenuManager::locationAction);
    connect(faces,       &QAction::triggered, this, &ContextMenuManager::facesAction);

    const bool groupsOnly = std::ranges::all_of(m_photos, &Photo::is<GroupInfo::Role::Representative>);
    const bool isSingleGroup = m_photos.size() == 1 && groupsOnly;
    const bool imagesOnly = std::ranges::all_of(m_photos | std::views::transform(qOverload<const Photo::Data &>(&Photo::getPath)), &MediaTypes::isImageFile) &&
                            std::ranges::all_of(m_photos, &Photo::is<GroupInfo::Role::None>);
    const bool isSingleImage = m_photos.size() == 1 && imagesOnly;

    groupPhotos->setEnabled(m_photos.size() > 1 && imagesOnly);
    manageGroup->setEnabled(isSingleGroup);
    ungroupPhotos->setEnabled(groupsOnly);
    location->setEnabled(m_photos.size() == 1);
    faces->setEnabled(m_enableFaceRecognition && isSingleImage);
    ungroupPhotos->setVisible(groupsOnly);

    std::vector<std::unique_ptr<QAction>> actions;
    actions.emplace_back(groupPhotos);
    actions.emplace_back(manageGroup);
    if (groupsOnly)
        actions.emplace_back(ungroupPhotos);
    actions.emplace_back(location);
    actions.emplace_back(faces);

    m_model.addActions(std::move(actions));
}


void ContextMenuManager::removeGroupOf(const std::vector<Photo::Data>& representatives)
{
    for (const Photo::Data& representative: representatives)
    {
        const GroupInfo& grpInfo = representative.groupInfo;
        const Group::Id gid = grpInfo.group_id;

        assert(gid.valid());

        Database::IDatabase& db = m_project->getDatabase();
        GroupsManager::ungroup(db, gid);

        // delete representative file
        QFile::remove(representative.path);
    }
}


void ContextMenuManager::groupPhotosAction()
{
    GroupsManager::groupIntoUnified(*m_project, m_photos);
}


void ContextMenuManager::manageGroupsAction()
{
    Database::IDatabase& db = m_project->getDatabase();


    const std::vector<Photo::Data> groupMembers = evaluate<std::vector<Photo::Data>(Database::IBackend &)>(db, [this](Database::IBackend& backend)
    {
        std::vector<Photo::Data> members;

        auto& groupOperator = backend.groupOperator();
        const auto memberIds = groupOperator.membersOf(m_photos.front().groupInfo.group_id);

        for (const auto& id: memberIds)
        {
            const Photo::Data member = backend.getPhoto(id);
            members.push_back(member);
        }

        return members;
    });

    IExifReaderFactory& factory = m_core->getExifReaderFactory();
    auto logger = m_core->getLoggerFactory().get("PhotosGrouping");
    PhotosGroupingDialog dialog(groupMembers, factory, m_core->getTaskExecutor(), m_core->getConfiguration(), logger.get());
    const int status = dialog.exec();

    if (status == QDialog::Accepted)
    {
        // remove old group
        removeGroupOf(m_photos);

        // create new one
        const QString representantPath = GroupsManager::includeRepresentatInDatabase(dialog.getRepresentative(), *m_project);
        GroupsManager::group(db, groupMembers, representantPath, dialog.groupType());
    }
}


void ContextMenuManager::ungroupAction()
{
    removeGroupOf(m_photos);
}


void ContextMenuManager::locationAction()
{
    const Photo::Data& first = m_photos.front();
    const QString relative_path = first.path;
    const QString absolute_path = m_project->makePathAbsolute(relative_path);
    const QFileInfo photoFileInfo(absolute_path);
    const QString file_dir = photoFileInfo.path();

    QDesktopServices::openUrl(QUrl::fromLocalFile(file_dir));
}


void ContextMenuManager::facesAction()
{
    const Photo::Data& first = m_photos.front();

    FacesDialog faces_dialog(first, m_core, m_project);
    faces_dialog.exec();
}
