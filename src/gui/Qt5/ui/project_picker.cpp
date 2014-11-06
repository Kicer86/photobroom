
#include "project_picker.hpp"
#include "ui_project_picker.h"

#include <QStringListModel>

#include "components/project_creator/project_creator_dialog.hpp"

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
}


QString ProjectPicker::choosenProjectName() const
{
    return m_choosenProjectName;
}


void ProjectPicker::on_openButton_clicked()
{
    done(QDialog::Accepted);
}


void ProjectPicker::on_newButton_clicked()
{
    ProjectCreator prjCreator;

    const int r = prjCreator.create(m_prjManager, m_pluginLoader);

    if (r == QDialog::Accepted)
    {}
}


void ProjectPicker::on_deleteButton_clicked()
{

}
