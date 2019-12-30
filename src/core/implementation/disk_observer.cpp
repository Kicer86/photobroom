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

#include "disk_observer.hpp"

#include <QFileSystemWatcher>
#include <QTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QEventLoopLocker>


Watcher::Watcher(const QString& filePath, QEventLoopLocker* locker, QObject* p):
    QThread(p),
    m_filePath(filePath),
    m_fsWatcher(new QFileSystemWatcher(this)),
    m_timer(new QTimer(this)),
    m_locker(locker),
    m_cv()
{
    const QFileInfo filePathInfo(m_filePath);
    const QString dir = filePathInfo.absolutePath();

    m_fsWatcher->addPath(dir);

    m_timer->setSingleShot(true);
    m_timer->setInterval(3000);
    m_timer->start();
}


Watcher::~Watcher()
{
}


void Watcher::run()
{
    connect( m_fsWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(dirChanged(QString)));
    connect( m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    
    QFileInfo fileInfo(m_filePath);

    //wait for file to appear
    std::mutex m;
    std::unique_lock<std::mutex> l(m);
    m_cv.wait(l, [&]
    {
        return fileInfo.exists();
    });

    m_locker.reset();
}


void Watcher::timeout()
{
    m_cv.notify_one();
}


void Watcher::dirChanged(const QString &)
{
    m_cv.notify_one();
}


DiskObserver::DiskObserver(): QObject()
{

}


DiskObserver::~DiskObserver()
{

}


bool DiskObserver::waitForFileToAppear(const QString& path)
{
    QEventLoop loop;
    QEventLoopLocker* locker = new QEventLoopLocker(&loop);
    QFileInfo fInfo(path);
    Watcher watcher(path, locker);
    watcher.start();

    loop.exec();
    watcher.wait();

    return fInfo.exists();
}

