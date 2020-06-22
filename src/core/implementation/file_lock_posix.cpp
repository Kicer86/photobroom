/*
 * Photo Broom - photos management tool.
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

#include "file_lock.hpp"

#include <cassert>
#include <fcntl.h>
#include <unistd.h>


namespace
{
    struct Impl
    {
        int m_fd;

        Impl(int fd): m_fd(fd)
        {

        }
    };
}


bool FileLock::tryLock()
{
    assert(m_impl == nullptr);

    const int fd = open(m_path.toStdString().c_str(), O_CREAT | O_WRONLY);

    if (fd != -1)
    {
        struct flock fl;

        fl.l_type   = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start  = 0;
        fl.l_len    = 0;
        fl.l_pid    = getpid();

        const int lock_result = fcntl(fd, F_SETLK, &fl);

        if (lock_result == -1)
            close(fd);
        else
            m_impl = new Impl(fd);
    }

    return m_impl != nullptr;
}


void FileLock::unlock()
{
    if (m_impl != nullptr)
    {
        Impl* impl = static_cast<Impl *>(m_impl);

        struct flock fl;

        fl.l_type   = F_UNLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start  = 0;
        fl.l_len    = 0;
        fl.l_pid    = getpid();

        fcntl(impl->m_fd, F_SETLK, &fl);
        close(impl->m_fd);
        unlink(m_path.toStdString().c_str());

        delete impl, m_impl = nullptr;
    }
}
