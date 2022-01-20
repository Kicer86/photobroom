
#ifndef CONTEXT_MENU_MANAGER_HPP_INCLUDED
#define CONTEXT_MENU_MANAGER_HPP_INCLUDED


#include <QObject>

#include <project_utils/project.hpp>
#include "models/actions_model.hpp"
#include "utils/selection_to_photoid_translator.hpp"


class ContextMenuManager: public QObject
{
    Q_OBJECT

    Q_PROPERTY(const QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(QList<QVariant> selection READ selection WRITE setSelection NOTIFY selectionChanged)
    Q_PROPERTY(Project* project READ project WRITE setProject)

public:
    ContextMenuManager();

    const QAbstractItemModel* model() const;
    QList<QVariant> selection() const;
    Project* project() const;

    void setSelection(const QList<QVariant> &);
    void setProject(Project *);

signals:
    void selectionChanged(const QList<QVariant> &);

private:
    std::unique_ptr<SelectionToPhotoDataTranslator> m_translator;
    std::vector<Photo::Data> m_photos;
    ActionsModel m_model;
    QList<QVariant> m_selection;
    Project* m_project;
    const bool m_enableFaceRecognition;

    void updateModel(const std::vector<Photo::Data> &);
    void groupPhotosAction();
};

#endif
