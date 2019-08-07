/*
 * Dialog for series detection
 * Copyright (C) 2019  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "series_detection.hpp"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>

#include <core/iexif_reader.hpp>
#include <database/idatabase.hpp>


using namespace std::placeholders;

SeriesDetection::SeriesDetection(Database::IDatabase* db, IExifReaderFactory* exif):
    QDialog(),
    m_tabModel(new QStandardItemModel(this)),
    m_exif(exif)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QGroupBox* detected = new QGroupBox(tr("Detected series"), this);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(detected);
    layout->addWidget(buttons);

    QHBoxLayout* detectedLayout = new QHBoxLayout(detected);
    QTableView* tabView = new QTableView(detected);
    tabView->setModel(m_tabModel);

    detectedLayout->addWidget(tabView);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    auto callback = m_callback_mgr.make_safe_callback<void(Database::IBackend* backend)>(std::bind(&SeriesDetection::fetch_series, this, _1));

    db->exec(callback);
}


SeriesDetection::~SeriesDetection()
{
    m_callback_mgr.invalidate();
}


void SeriesDetection::fetch_series(Database::IBackend* backend)
{
    SeriesDetector detector(backend, m_exif->get());

    const auto detected = detector.listDetections();

    // go back to main thread
    invokeMethod(this, &SeriesDetection::load_series, detected);
}


void SeriesDetection::load_series(const std::vector<SeriesDetector::Detection>& detections)
{
    for(const SeriesDetector::Detection& detection: detections)
    {
        QList<QStandardItem *> row;

        QString type;
        switch (detection.type)
        {
            case Group::Type::Invalid:                           break;
            case Group::Type::Animation: type = tr("Animation"); break;
            case Group::Type::HDR:       type = tr("HDR");       break;
        }

        row.append(new QStandardItem(type));
        row.append(new QStandardItem(QString::number(detection.members.size())));

        m_tabModel->appendRow(row);
    }
}
