/*
 * File lock mechanism
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef FILELOCK_HPP
#define FILELOCK_HPP

#include <QString>

#include "core_export.h"


class CORE_EXPORT FileLock
{
    public:
        explicit FileLock(const QString& file_path);
        ~FileLock();

        FileLock(const FileLock &) = delete;
        FileLock& operator=(const FileLock &) = delete;

        bool tryLock();
        void unlock();

    private:
        QString m_path;
        void* m_impl;
};

#endif // FILELOCK_HPP
