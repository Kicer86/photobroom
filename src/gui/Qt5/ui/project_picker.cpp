
#include "project_picker.hpp"
#include "ui_project_picker.h"

#include <cassert>

#include <QStandardItemModel>

#include "components/project_creator/project_creator_dialog.hpp"
#include <project_utils/iproject_manager.hpp>

ProjectPicker::ProjectPicker(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::ProjectPicker),
    m_choosenProject(),
    m_model(nullptr),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_projs()
{
    ui->setupUi(this);

    m_model = new QStandardItemModel(this);
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


ProjectInfo ProjectPicker::choosenProject() const
{
    return m_choosenProject;
}


void ProjectPicker::on_openButton_clicked()
{
    m_choosenProject = selectedPrj();

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
    const ProjectInfo prj = selectedPrj();
    m_prjManager->remove(prj);

    reload();
}


void ProjectPicker::reload()
{
    m_projs.clear();
    m_model->clear();
    const auto prjs = m_prjManager->listProjects();

    for(const ProjectInfo& prjInfo: prjs)
    {
        m_projs[prjInfo.id] = prjInfo;

        QStandardItem* item = new QStandardItem(prjInfo.name);
        item->setData(prjInfo.id);

        m_model->appendRow(item);
    }
}


ProjectInfo ProjectPicker::selectedPrj() const
{
    ProjectInfo prjInfo;

    QItemSelectionModel* selection = ui->projectsList->selectionModel();
    const QModelIndexList indexes = selection->selectedIndexes();

    if (indexes.empty() == false)
    {
        //get selected index
        const QModelIndex& selected = indexes.first();

        //find project name in model
        const QVariant id_raw = selected.data(Qt::UserRole + 1);
        const QString id = id_raw.toString();

        auto it = m_projs.find(id);
        assert(it != m_projs.end());

        if (it != m_projs.end())
            prjInfo = it->second;
    }

    return prjInfo;
}

