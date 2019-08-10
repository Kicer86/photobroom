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

#include <core/athumbnail_manager.hpp>
#include <core/iexif_reader.hpp>
#include <database/idatabase.hpp>


using namespace std::placeholders;

SeriesDetection::SeriesDetection(Database::IDatabase* db, IExifReaderFactory* exif, AThumbnailManager* thmMgr):
    QDialog(),
    m_tabModel(new QStandardItemModel(this)),
    m_exif(exif),
    m_thmMgr(thmMgr)
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

    m_tabModel->setHorizontalHeaderLabels( {tr("preview"), tr("type"), tr("photos")} );

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

    std::vector<ExDetection> ex_detections;
    // collect one photo path for each detection
    for(const SeriesDetector::Detection& detection: detected)
    {
        const Photo::Data pd = backend->getPhoto(detection.members.front());
        const ExDetection ex_detection { {detection.type, detection.members}, pd.path};

        ex_detections.push_back(ex_detection);
    }

    // go back to main thread
    invokeMethod(this, &SeriesDetection::load_series, ex_detections);
}


void SeriesDetection::load_series(const std::vector<ExDetection>& detections)
{
    for(std::size_t i = 0; i < detections.size(); i++)
    {
        const ExDetection& detection = detections[i];

        auto setThumbnailCallback = make_cross_thread_function<int, const QImage &>(this, std::bind(&SeriesDetection::setThumbnail, this, i, _1, _2));
        auto setThumbnailCallbackSafe = m_callback_mgr.make_safe_callback<void(int, const QImage &)>(setThumbnailCallback);
        m_thmMgr->fetch(detection.path, 64, setThumbnailCallbackSafe);

        QList<QStandardItem *> row;

        QString type;
        switch (detection.type)
        {
            case Group::Type::Invalid:                           break;
            case Group::Type::Animation: type = tr("Animation"); break;
            case Group::Type::HDR:       type = tr("HDR");       break;
        }

        row.append(new QStandardItem);                  // placeholder for image
        row.append(new QStandardItem(type));
        row.append(new QStandardItem(QString::number(detection.members.size())));

        m_tabModel->appendRow(row);
    }
}

void SeriesDetection::setThumbnail(int row, int /* height */, const QImage& img)
{
    QStandardItem* item = m_tabModel->item(row, 0);
    const QPixmap pixmap = QPixmap::fromImage(img);
    const QIcon icon(pixmap);

    item->setIcon(icon);
}
