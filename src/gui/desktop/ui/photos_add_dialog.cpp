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
#include <QMessageBox>
#include <QPainter>

#include <configuration/iconfiguration.hpp>

#include "models/decorated_image_list_model.hpp"
#include "models/staged_photos_data_model.hpp"
#include "views/tree_item_delegate.hpp"
#include "ui_photos_add_dialog.h"


namespace
{
    struct StatusDelegate: QAbstractItemDelegate
    {
        StatusDelegate(QAbstractItemDelegate* delegate): m_delegate(delegate)
        {

        }

        // QAbstractItemDelegate interface
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
        {
            m_delegate->paint(painter, option, index);

            painter->drawLine(0, 0, 100, 100);
        }

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
        {
            return m_delegate->sizeHint(option, index);
        }
        //

        QAbstractItemDelegate* m_delegate;
    };
}


PhotosAddDialog::PhotosAddDialog(IConfiguration* config, QWidget *parent):
    QDialog(parent),
    ui(new Ui::PhotosAddDialog),
    m_config(config),
    m_treeModel(nullptr),
    m_photosCollector(),
    m_dirContentModel(nullptr),
    m_stagedModel(nullptr)
{
    ui->setupUi(this);

    // setup photos tree browser
    m_treeModel = new QFileSystemModel(this);
    m_treeModel->setRootPath(QDir::homePath());
    m_treeModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    ui->browseTree->setModel(m_treeModel);

    // on Linux hide '/' node, it looks ridiculous
#ifdef OS_UNIX
    ui->browseTree->setRootIndex(m_treeModel->index("/"));
#endif

    //model for list view
    m_dirContentModel = new DecoratedImageListModel(this);

    // model for staged photos view
    m_stagedModel = new StagedPhotosDataModel(this);
    ui->stagedPhotosView->setModel(m_stagedModel);

    //
    ui->browseList->setSelectionMode(QAbstractItemView::MultiSelection);

    //setup tag editor
    ui->tagsEditor->set(ui->stagedPhotosView->selectionModel());
    ui->tagsEditor->set(m_stagedModel);

    // load layout
    const QVariant geometry = m_config->getEntry("photos_add_dialog::geometry");
    if (geometry.isValid())
    {
        const QByteArray base64 = geometry.toByteArray();
        const QByteArray geometryData = QByteArray::fromBase64(base64);
        restoreGeometry(geometryData);
    }

    TreeItemDelegate* treeItemDelegate = new TreeItemDelegate(ui->browseList);
    StatusDelegate* statusDelegate = new StatusDelegate(treeItemDelegate);
    ui->browseList->setItemDelegate(statusDelegate);
    ui->stagedPhotosView->setItemDelegate(new TreeItemDelegate(ui->stagedPhotosView));

    //expand home dir
    for(QModelIndex item = m_treeModel->index(QDir::homePath()); item.isValid(); item = item.parent())
        ui->browseTree->expand(item);

    //attach to selection updates
    connect(ui->browseTree->selectionModel(), &QItemSelectionModel::currentChanged, this, &PhotosAddDialog::treeSelectionChanged);
    connect(ui->stagedPhotosView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PhotosAddDialog::stagedTreeSelectionChanged);
    connect(this, &PhotosAddDialog::updateLoadValue, ui->loadProgressValue, &QLabel::setText);

    //
    connect(m_treeModel, &QFileSystemModel::directoryLoaded, this, &PhotosAddDialog::treeLoaded);
}


PhotosAddDialog::~PhotosAddDialog()
{
    delete ui;
}


void PhotosAddDialog::set(ITaskExecutor* executor)
{
    m_dirContentModel->set(executor);
    m_stagedModel->set(executor);
}


void PhotosAddDialog::set(Database::IDatabase* database)
{
    m_stagedModel->setDatabase(database);
}


void PhotosAddDialog::set(IPhotosManager* photosManager)
{
    m_dirContentModel->set(photosManager);
}


void PhotosAddDialog::treeSelectionChanged(const QModelIndex& current, const QModelIndex &)
{
    // crawler may already work for another dir, disconnect from it and stop it
    m_photosCollector.disconnect(this);
    m_photosCollector.stop();

    // Disconnect list model from list view.
    // That's because model will be modified from another thread and horrible things may happen.
    ui->browseList->setModel(nullptr);
    m_dirContentModel->clear();

    const QString path = m_treeModel->filePath(current);

    //init load info
    ui->loadWidget->setVisible(true);

    //connect to 'finished' notification
    connect(&m_photosCollector, &PhotosCollector::finished, this, &PhotosAddDialog::browseListFilled);

    m_photosCollector.collect(path, [&](const QString& photo_path)
    {
        m_dirContentModel->insert(photo_path);

        const QString value = QString::number(m_dirContentModel->rowCount());

        emit updateLoadValue(value);
    });
}


void PhotosAddDialog::listSelectionChanged(const QItemSelection& selected, const QItemSelection &)
{
    const bool enable = selected.empty() == false;

    ui->addSelectionButton->setEnabled(enable);
}


void PhotosAddDialog::browseListFilled()
{
    ui->browseList->setModel(m_dirContentModel);
    ui->loadWidget->setHidden(true);

    connect(ui->browseList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PhotosAddDialog::listSelectionChanged, Qt::UniqueConnection);
}


void PhotosAddDialog::stagedTreeSelectionChanged(const QItemSelection& selected, const QItemSelection &)
{
    const bool enable = selected.empty() == false;

    ui->removeSelectedButton->setEnabled(enable);
}


void PhotosAddDialog::treeLoaded(const QString &)
{
    ui->browseTree->resizeColumnToContents(0);
}


void PhotosAddDialog::closeEvent(QCloseEvent* e)
{
    // store windows state
    const QByteArray geometry = saveGeometry();
    m_config->setEntry("photos_add_dialog::geometry", geometry.toBase64());

    QWidget::closeEvent(e);
}


void PhotosAddDialog::on_addSelectionButton_pressed()
{
    QItemSelectionModel* selectionModel = ui->browseList->selectionModel();

    const QItemSelection selection = selectionModel->selection();

    for(const QModelIndex& index: selection.indexes())
    {
        const QString path = m_dirContentModel->get(index);
        m_stagedModel->addPhoto(path);
    }
}


void PhotosAddDialog::on_buttonBox_accepted()
{
    m_stagedModel->storePhotos();

    accept();
}


void PhotosAddDialog::on_buttonBox_rejected()
{
    const QMessageBox::StandardButton result = QMessageBox::question(this, tr("Discard changes?"), tr("All changes will be lost, do you want to proceed?"));

    if (result == QMessageBox::Yes)
    {
        m_stagedModel->dropPhotos();

        reject();
    }
}


void PhotosAddDialog::on_removeSelectedButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->stagedPhotosView->selectionModel();

    const QItemSelection selection = selectionModel->selection();

    for(const QModelIndex& index: selection.indexes())
        if (m_stagedModel->isPhoto(index))
            m_stagedModel->dropPhoto(index);
}
