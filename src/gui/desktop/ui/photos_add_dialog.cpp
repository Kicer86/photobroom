/*
 * Dialog for photos adding.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */


#include "photos_add_dialog.hpp"

#include <QFileSystemModel>

#include <configuration/iconfiguration.hpp>

#include "ui_photos_add_dialog.h"


PhotosAddDialog::PhotosAddDialog(IConfiguration* config, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::PhotosAddDialog),
    m_config(config)
{
    ui->setupUi(this);

    // setup photos tree browser
    m_treeModel = new QFileSystemModel(this);
    m_treeModel->setRootPath(QDir::homePath());
    m_treeModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    ui->browseTree->setModel(m_treeModel);

    // on Linux hide '/' node, it look ridiculous
#ifdef OS_UNIX
    ui->browseTree->setRootIndex(m_treeModel->index("/"));
#endif

    // load layout
    const QVariant geometry = m_config->getEntry("photos_add_dialog::geometry");
    if (geometry.isValid())
    {
        const QByteArray base64 = geometry.toByteArray();
        const QByteArray geometryData = QByteArray::fromBase64(base64);
        restoreGeometry(geometryData);
    }

    const QVariant state = m_config->getEntry("photos_add_dialog::state");
    if (state.isValid())
    {
        const QByteArray base64 = state.toByteArray();
        const QByteArray stateData = QByteArray::fromBase64(base64);
        restoreState(stateData);
    }

    //expand home dir
    for(QModelIndex item = m_treeModel->index(QDir::homePath()); item.isValid(); item = item.parent())
        ui->browseTree->expand(item);

    //attach to selection updates
    QItemSelectionModel* selection = ui->browseTree->selectionModel();

    connect(selection, &QItemSelectionModel::currentChanged, this, &PhotosAddDialog::treeSelectionChanged);
}


PhotosAddDialog::~PhotosAddDialog()
{
    delete ui;
}


void PhotosAddDialog::treeSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    const QString path = m_treeModel->filePath(current);

}


void PhotosAddDialog::closeEvent(QCloseEvent* e)
{
    // store windows state
    const QByteArray geometry = saveGeometry();
    m_config->setEntry("photos_add_dialog::geometry", geometry.toBase64());

    const QByteArray state = saveState();
    m_config->setEntry("photos_add_dialog::state", state.toBase64());

    emit closing();

    QWidget::closeEvent(e);
}
