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

#include <set>

#include <QDialog>

#include <database/idatabase.hpp>
#include "utils/photos_collector.hpp"

class QLabel;


namespace
{
    struct ListExistingPhotos final: Database::AGetPhotosTask
    {
        ListExistingPhotos(const std::function<void(const IPhotoInfo::List& photos)>& callback): m_callback(callback)
        {
        }

        void got(const IPhotoInfo::List& photos) override
        {
            m_callback(photos);
        }

        std::function<void(const IPhotoInfo::List& photos)> m_callback;
    };
}


class CollectionDirScanDialog: public QDialog
{
        Q_OBJECT

    public:
        CollectionDirScanDialog(const QString& collectionLocation, Database::IDatabase *, QWidget* parent = nullptr);
        CollectionDirScanDialog(const CollectionDirScanDialog &) = delete;
        ~CollectionDirScanDialog();

        CollectionDirScanDialog& operator=(const CollectionDirScanDialog &) = delete;

        const std::set<QString>& newPhotos() const;

    private:
        enum class State
        {
            Canceled,
            Scanning,
            Analyzing,
            Done,
        };

        PhotosCollector m_collector;
        std::set<QString> m_photosFound;
        IPhotoInfo::List m_dbPhotos;
        const QString m_collectionLocation;
        State m_state;
        QLabel* m_info;
        QPushButton* m_button;
        Database::IDatabase* m_database;
        std::atomic<bool> m_gotPhotos;
        std::atomic<bool> m_gotDBPhotos;

        // slots:
        void buttonPressed();
        void scanDone();
        void performAnalysis();
        //

        void scan();
        void checkIfReady();

        void gotPhoto(const QString &);
        void gotExistingPhotos(const IPhotoInfo::List &);
        void updateGui();

    signals:
        void analyze();
};

#endif // COLLECTIONDIRSCANDIALOG_HPP
