
#ifndef CONTEXT_MENU_MANAGER_HPP_INCLUDED
#define CONTEXT_MENU_MANAGER_HPP_INCLUDED


#include <QObject>
#include <QtQml/qqmlregistration.h>

#include <core/icore_factory_accessor.hpp>
#include <database/database_tools/id_to_data_converter.hpp>
#include <database/explicit_photo_delta.hpp>
#include <project_utils/project.hpp>
#include "models/actions_model.hpp"


class ContextMenuManager: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(const QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(QList<QVariant> selection READ selection WRITE setSelection NOTIFY selectionChanged)
    Q_PROPERTY(Project* project READ project WRITE setProject REQUIRED)
    Q_PROPERTY(ICoreFactoryAccessor* coreFactory READ coreFactory WRITE setCoreFactory REQUIRED)

public:
    ContextMenuManager();

    const QAbstractItemModel* model() const;
    QList<QVariant> selection() const;
    Project* project() const;
    ICoreFactoryAccessor* coreFactory() const;

    void setSelection(const QList<QVariant> &);
    void setProject(Project *);
    void setCoreFactory(ICoreFactoryAccessor *);

signals:
    void selectionChanged(const QList<QVariant> &);

    // actions to be handled by qml side.
    // TODO: I do not like the fact that some actions are handled on cpp side
    //       and some on the qml side. At this moment I could not find another solution
    void faceRecognitionAction();

private:
    using ExplicitDelta = Photo::ExplicitDelta<Photo::Field::Path, Photo::Field::GroupInfo>;

    std::unique_ptr<IdToDataConverter> m_translator;
    std::vector<ExplicitDelta> m_photos;
    ActionsModel m_model;
    QList<QVariant> m_selection;
    Project* m_project = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;
    const bool m_enableFaceRecognition;

    void updateModel(const std::vector<Photo::DataDelta> &);
    void removeGroupOf(const std::vector<ExplicitDelta>&);

    void groupPhotosAction();
    void manageGroupsAction();
    void ungroupAction();
    void locationAction();
    void facesAction();
};

#endif
