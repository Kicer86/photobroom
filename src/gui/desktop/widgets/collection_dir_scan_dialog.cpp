/*
 * Dialog for collection scanning
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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


#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QTimer>

#include "collection_dir_scan_dialog.hpp"


CollectionDirScanDialog::CollectionDirScanDialog(const QString& collectionLocation, QWidget* p):
    QDialog(p),
    m_collector(),
    m_curPathStr(),
    m_mutex(),
    m_info(nullptr),
    m_curPath(nullptr),
    m_button(nullptr),
    m_close(false),
    m_canceled(false)
{
    m_info = new QLabel(tr("Collection scan in progress"), this);
    m_button = new QPushButton(tr("Cancel"), this);

    connect(m_button, &QPushButton::clicked, this, &CollectionDirScanDialog::buttonPressed);
    connect(&m_collector, &PhotosCollector::finished, this, &CollectionDirScanDialog::scanDone);

    // path
    QLabel* pathInfo = new QLabel(tr("Scanning:"), this);
    m_curPath = new QLabel(this);

    QHBoxLayout* pathLayout = new QHBoxLayout;
    pathLayout->addWidget(pathInfo);
    pathLayout->addWidget(m_curPath);

    // main layout
    QVBoxLayout* l = new QVBoxLayout(this);
    l->addLayout(pathLayout);
    l->addStretch();
    l->addWidget(m_info);
    l->addWidget(m_button);

    // gui updater
    m_guiUpdater = new QTimer(this);
    connect(m_guiUpdater, &QTimer::timeout, this, &CollectionDirScanDialog::updateGui);
    m_guiUpdater->start(1000);

    //
    scan(collectionLocation);
}


CollectionDirScanDialog::~CollectionDirScanDialog()
{

}


void CollectionDirScanDialog::buttonPressed()
{
    if (m_close)
        accept();
    else
    {
        m_canceled = true;
        m_collector.stop();
    }
}


void CollectionDirScanDialog::scanDone()
{
    if (m_canceled)
        m_info->setText(tr("Collection scan canceled"));
    else
        m_info->setText(tr("Collection scan finished"));

    m_button->setText(tr("Close"));
    m_close = true;
    m_guiUpdater->stop();
}


void CollectionDirScanDialog::updateGui()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_curPath->setText(m_curPathStr);
}


void CollectionDirScanDialog::scan(const QString& location)
{
    using namespace std::placeholders;
    auto callback = std::bind(&CollectionDirScanDialog::gotPhoto, this, _1);

    m_collector.collect(location, callback);
}


void CollectionDirScanDialog::gotPhoto(const QString& path)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    const QFileInfo info(path);
    m_curPathStr = info.absolutePath();
}
