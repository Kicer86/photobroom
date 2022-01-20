
#include <ranges>
#include <QFile>

#include <core/media_types.hpp>
#include <database/photo_utils.hpp>
#include <face_recognition/face_recognition.hpp>
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
        m_translator = std::make_unique<SelectionToPhotoDataTranslator>(m_project->getDatabase());
        connect(m_translator.get(), &SelectionToPhotoDataTranslator::selectionChanged,
                this, &ContextMenuManager::updateModel);
    }
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


void ContextMenuManager::groupPhotosAction()
{
    GroupsManager::groupIntoUnified(*m_project, m_photos);
}
