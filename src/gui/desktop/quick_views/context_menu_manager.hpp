
#ifndef CONTEXT_MENU_MANAGER_HPP_INCLUDED
#define CONTEXT_MENU_MANAGER_HPP_INCLUDED


#include <QObject>

#include <core/icore_factory_accessor.hpp>
#include <project_utils/project.hpp>
#include "models/actions_model.hpp"
#include "utils/id_to_data_converter.hpp"


class ContextMenuManager: public QObject
{
    Q_OBJECT

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

private:
    std::unique_ptr<SelectionToPhotoDataTranslator> m_translator;
    std::vector<Photo::Data> m_photos;
    ActionsModel m_model;
    QList<QVariant> m_selection;
    Project* m_project = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;
    const bool m_enableFaceRecognition;

    void updateModel(const std::vector<Photo::Data> &);
    void removeGroupOf(const std::vector<Photo::Data>&);

    void groupPhotosAction();
    void manageGroupsAction();
    void ungroupAction();
    void locationAction();
    void facesAction();
};

#endif
