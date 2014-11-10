
#include "project_picker.hpp"
#include "ui_project_picker.h"

#include <cassert>

#include <QStringListModel>

#include "components/project_creator/project_creator_dialog.hpp"
#include <project_utils/iproject_manager.hpp>

ProjectPicker::ProjectPicker(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::ProjectPicker),
    m_choosenProjectName(),
    m_model(nullptr),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr)
{
    ui->setupUi(this);

    m_model = new QStringListModel(this);
    ui->projectsList->setModel(m_model);
}


ProjectPicker::~ProjectPicker()
{
    delete ui;
}


void ProjectPicker::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void ProjectPicker::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;

    reload();
}


QString ProjectPicker::choosenProjectName() const
{
    return m_choosenProjectName;
}


void ProjectPicker::on_openButton_clicked()
{
    m_choosenProjectName = selectedPrj();

    done(QDialog::Accepted);
}


void ProjectPicker::on_newButton_clicked()
{
    ProjectCreator prjCreator;

    const int r = prjCreator.create(m_prjManager, m_pluginLoader);

    if (r == QDialog::Accepted)
        reload();
}


void ProjectPicker::on_deleteButton_clicked()
{

}


void ProjectPicker::reload()
{
    const auto prjs = m_prjManager->listProjects();

    m_model->setStringList(prjs);
}


QString ProjectPicker::selectedPrj() const
{
    QString name;

    QItemSelectionModel* selection = ui->projectsList->selectionModel();
    const QModelIndexList indexes = selection->selectedIndexes();

    if (indexes.empty() == false)
    {
        //get selected index
        const QModelIndex& selected = indexes.first();

        //find project name in model
        QStringList projects = m_model->stringList();

        assert(projects.size() > selected.row());

        name = projects[selected.row()];
    }

    return name;
}

