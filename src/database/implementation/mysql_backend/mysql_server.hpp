/*
 * A class for managing mysql server process.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef MYSQLSERVER_H
#define MYSQLSERVER_H

#include <memory>
#include <condition_variable>
#include <mutex>

#include <QString>
#include <QObject>

class QProcess;
class QFileSystemWatcher;

struct DiskObserver: public QObject
{
    Q_OBJECT

public:
    DiskObserver(const QString &);
    DiskObserver(const DiskObserver &) = delete;
    ~DiskObserver();

    DiskObserver& operator=(const DiskObserver &) = delete;

    bool waitForChange();

private slots:
    void dirChanged(const QString &);

private:
    std::condition_variable m_semaphore;
    std::mutex m_mutex;
    QFileSystemWatcher* m_watcher;
    QString m_socketPath;
};


class MySqlServer
{
    public:
        MySqlServer();
        MySqlServer(const MySqlServer &) = delete;
        ~MySqlServer();

        MySqlServer& operator=(const MySqlServer &) = delete;
        bool operator==(const MySqlServer &) = delete;

        QString run_server(const QString &);  //returns socket or null if failed

    private:
        std::unique_ptr<QProcess> m_serverProcess;

        std::string getDaemonPath() const;
        bool initDB(const std::string &, const std::string &) const;
        bool createConfig(const QString &) const;
        bool waitForServerToStart(const QString &) const;
        QString startProcess(const QString &, const QString &) const;
};

#endif // MYSQLSERVER_H
