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
#include <QStandardItemModel>

#include <configuration/iconfiguration.hpp>

#include "ui_photos_add_dialog.h"


PhotosAddDialog::PhotosAddDialog(IConfiguration* config, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::PhotosAddDialog),
    m_config(config),
    m_photosCollector(),
    m_browseModel(nullptr)
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

    //model for list view
    m_browseModel = new QStandardItemModel(this);

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
    // crawler may already work for another dir, disconnect from it and stop it
    m_photosCollector.disconnect(this);
    m_photosCollector.stop();

    // Disconnect list model from list view.
    // That's because model will be modified from another thread and horrible things may happen.
    ui->browseList->setModel(nullptr);
    m_browseModel->clear();

    const QString path = m_treeModel->filePath(current);

    //init progress bar
    ui->loadProgressBar->setEnabled(true);

    //connect to 'finished' notification
    connect(&m_photosCollector, &PhotosCollector::finished, this, &PhotosAddDialog::browseListFilled);

    m_photosCollector.collect(path, [&](const QString& photo_path)
    {
        const QPixmap pixmap(photo_path);
        const QPixmap scaled = pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QStandardItem* item = new QStandardItem(scaled, photo_path);

        m_browseModel->appendRow(item);
    });
}


void PhotosAddDialog::browseListFilled()
{
    ui->browseList->setModel(m_browseModel);
    ui->loadProgressBar->setDisabled(true);
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
