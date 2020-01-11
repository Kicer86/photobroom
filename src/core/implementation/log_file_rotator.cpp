/*
 * Photo Broom - photos management tool.
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


#include "log_file_rotator.hpp"

#include <QDir>
#include <QFileInfo>

LogFileRotator::LogFileRotator()
{

}


LogFileRotator::~LogFileRotator()
{

}


void LogFileRotator::rotate(const QString &logPath) const
{
    const QFileInfo info(logPath);
    const QDir dir = info.absoluteDir();
    const QString name = info.fileName();
    const QString path = info.absolutePath();
    QStringList entries = dir.entryList( {name, name + ".*"}, QDir::Files);

    // sort in reverse order: .log.5, .log.4, ..., .log
    std::sort(entries.rbegin(), entries.rend());

    for (const QString& entry: entries)
    {
        const QFileInfo entryInfo(entry);
        const QString ext = entryInfo.suffix();                 // .1, .2, ... or .log

        if (ext != "5")
        {
            const QString version = ext == "log"? 0: ext;      // 0 for ".log"
            const QString newName = name + "." + increaseVersion(version);

            QFile::remove(path + "/" + newName);                      // delete file to be overwritten
            QFile::rename(path + "/" + entry, path + "/" + newName);
        }
    }
}


QString LogFileRotator::increaseVersion(const QString& ver) const
{
    int v = ver.toInt();
    v++;

    return QString::number(v);
}
