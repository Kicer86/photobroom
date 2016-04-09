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


#include <QFileInfo>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "collection_dir_scan_dialog.hpp"


CollectionDirScanDialog::CollectionDirScanDialog(const QString& collectionLocation, QWidget* p):
    QDialog(p),
    m_collector(),
    m_photosFound(),
    m_photosFoundMutex(),
    m_state(State::Scanning),
    m_info(nullptr),
    m_button(nullptr)
{
    m_info = new QLabel(this);
    m_button = new QPushButton(this);

    connect(m_button, &QPushButton::clicked, this, &CollectionDirScanDialog::buttonPressed);
    connect(&m_collector, &PhotosCollector::finished, this, &CollectionDirScanDialog::scanDone);

    // main layout
    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(m_info);
    l->addWidget(m_button);

    //
    scan(collectionLocation);
}


CollectionDirScanDialog::~CollectionDirScanDialog()
{

}


void CollectionDirScanDialog::buttonPressed()
{
    if (m_state == State::Done || m_state == State::Canceled)
        accept();
    else
    {
        m_state = State::Canceled;
        m_collector.stop();

        updateGui();
    }
}


void CollectionDirScanDialog::scanDone()
{
    m_state = State::Analyzing;
    updateGui();
}


void CollectionDirScanDialog::scan(const QString& location)
{
    m_state = State::Scanning;
    // collect photos from disk
    using namespace std::placeholders;
    auto callback = std::bind(&CollectionDirScanDialog::gotPhoto, this, _1);

    m_collector.collect(location, callback);

    updateGui();
}


void CollectionDirScanDialog::gotPhoto(const QString& path)
{
    std::lock_guard<std::mutex> photos_lock(m_photosFoundMutex);

    m_photosFound.insert(path);
}


void CollectionDirScanDialog::updateGui()
{
    switch(m_state)
    {
        case State::Canceled:
            m_info->setText(tr("Collection scan canceled"));
            m_button->setText(tr("Close"));
            break;

        case State::Scanning:
            m_info->setText(tr("Collection scan in progress"));
            m_button->setText(tr("Cancel"));
            break;

        case State::Analyzing:
            m_info->setText(tr("Searching for new photos"));
            m_button->setText(tr("Cancel"));
            break;

        case State::Done:
            m_info->setText(tr("Done"));
            m_button->setText(tr("Close"));
            break;
    }
}
