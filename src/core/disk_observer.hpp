/*
 * Photo Broom - photos management tool.
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

#ifndef DISKOBSERVER_HPP
#define DISKOBSERVER_HPP

#include <memory>
#include <condition_variable>

#include <QThread>

#include "core_export.h"

class QEventLoopLocker;
class QFileSystemWatcher;
class QTimer;

class Watcher: public QThread
{
        Q_OBJECT

    public:
        Watcher(const QString &, QEventLoopLocker *, QObject* parent = 0);
        ~Watcher();
        Watcher(const Watcher &) = delete;

        Watcher& operator=(const Watcher &) = delete;

    private:
        const QString m_filePath;
        QFileSystemWatcher* m_fsWatcher;
        QTimer* m_timer;
        std::unique_ptr<QEventLoopLocker> m_locker;
        std::condition_variable m_cv;

        virtual void run() override;

    private slots:
        void timeout();
        void dirChanged(const QString &);
};


class CORE_EXPORT DiskObserver: public QObject
{
    Q_OBJECT

    public:
        DiskObserver();
        DiskObserver(const DiskObserver &) = delete;
        ~DiskObserver();

        DiskObserver& operator=(const DiskObserver &) = delete;

        bool waitForFileToAppear(const QString &);
};

#endif // DISKOBSERVER_HPP
