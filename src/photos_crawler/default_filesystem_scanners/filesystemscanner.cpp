/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "filesystemscanner.hpp"

#include <QDir>
#include <QDirIterator>


void FileSystemScanner::ignorePaths(const QStringList& to_ignore)
{
    m_ignored = to_ignore;
}


void FileSystemScanner::getFilesFor(const QString& dir_path, IFileNotifier* notifier)
{
    m_work = true;

    QDirIterator dirIt(dir_path,
                       QStringList(),
                       QDir::NoDotAndDotDot | QDir::Files,
                       QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while (m_work && dirIt.hasNext())
    {
        const QString entry = dirIt.next();
        const bool is_fine = std::none_of(m_ignored.begin(), m_ignored.end(), [&entry](const QString& ignored)
        {
            return entry.contains(ignored);
        });

        if (is_fine)
            notifier->found(entry);
    }

    notifier->finished();
}


void FileSystemScanner::stop()
{
    m_work = false;
}


FileSystemScanner::FileSystemScanner(): m_work(true)
{

}


FileSystemScanner::~FileSystemScanner()
{

}
