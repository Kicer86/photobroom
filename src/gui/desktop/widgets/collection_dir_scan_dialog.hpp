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

#ifndef COLLECTIONDIRSCANDIALOG_HPP
#define COLLECTIONDIRSCANDIALOG_HPP

#include <mutex>
#include <set>

#include <QDialog>

#include "utils/photos_collector.hpp"

class QLabel;

class CollectionDirScanDialog: public QDialog
{
        Q_OBJECT

    public:
        CollectionDirScanDialog(const QString& collectionLocation, QWidget* parent = nullptr);
        CollectionDirScanDialog(const CollectionDirScanDialog &) = delete;
        ~CollectionDirScanDialog();

        CollectionDirScanDialog& operator=(const CollectionDirScanDialog &) = delete;

    private:
        PhotosCollector m_collector;
        QString m_curPathStr;
        std::mutex m_curPathStrMutex;
        std::set<QString> m_photosFound;
        std::mutex m_photosFoundMutex;
        QLabel* m_info;
        QLabel* m_curPath;
        QPushButton* m_button;
        QTimer* m_guiUpdater;
        bool m_close;
        bool m_canceled;

        // slots:
        void buttonPressed();
        void scanDone();
        void updateGui();
        //

        void scan(const QString &);
        void gotPhoto(const QString &);
};

#endif // COLLECTIONDIRSCANDIALOG_HPP
