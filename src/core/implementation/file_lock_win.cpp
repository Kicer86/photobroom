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

#include <Windows.h>

namespace
{
    struct Impl
    {
        HANDLE m_file;

        explicit Impl(HANDLE file): m_file(file)
        {

        }
    };
}


bool FileLock::tryLock()
{
    assert(m_impl == nullptr);

    LPCWSTR wideName = reinterpret_cast<LPCWSTR>( m_path.utf16() );
    HANDLE file = CreateFileW(wideName,
                              GENERIC_WRITE,
                              0,
                              nullptr,
                              CREATE_NEW,
                              FILE_FLAG_DELETE_ON_CLOSE,
                              0
                             );

    if (file != INVALID_HANDLE_VALUE)
        m_impl = new Impl(file);

    return m_impl != nullptr;
}


void FileLock::unlock()
{
    if (m_impl != nullptr)
    {
        Impl* impl = static_cast<Impl *>(m_impl);

        CloseHandle(impl->m_file);

        delete impl, m_impl = nullptr;
    }
}
