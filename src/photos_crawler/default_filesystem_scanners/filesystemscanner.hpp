/*
    File system scanner
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef ANALYZER_FILESYSTEMSCANNER_HPP
#define ANALYZER_FILESYSTEMSCANNER_HPP

#include "../ifile_system_scanner.hpp"

#include <atomic>

#include "photos_crawler_export.h"

class PHOTOS_CRAWLER_EXPORT FileSystemScanner: public IFileSystemScanner
{

    public:
        FileSystemScanner();
        virtual ~FileSystemScanner();

        void getFilesFor(const QString &, IFileNotifier *) override;
        void stop() override;

    private:
        std::atomic<bool> m_work;

        FileSystemScanner(const FileSystemScanner& other) = delete;
        virtual FileSystemScanner& operator=(const FileSystemScanner& other) = delete;
        virtual bool operator==(const FileSystemScanner& other) const = delete;
};

#endif // FILESYSTEMSCANNER_HPP
