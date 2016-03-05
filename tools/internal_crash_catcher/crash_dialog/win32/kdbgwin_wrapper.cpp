/*
 * Win32 debugger
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


#include "kdbgwin_wrapper.hpp"

#include <cassert>
#include <iostream>

#include <Windows.h>

// based on drkonqi

Debugger::Debugger(): m_exec()
{

}


Debugger::~Debugger()
{

}


bool Debugger::init(qint64 pid, qint64 tid, const QString &exec)
{
    m_pid = pid;
    m_tid = tid;
    m_exec = exec;

    const bool status = enableDebugPrivilege();

    return status;
}


void Debugger::requestBacktrace(const std::function<void (const std::vector<QString> &)> &)
{

}


const QString& Debugger::exec() const
{
    return m_exec;
}


bool Debugger::enableDebugPrivilege()
{
    std::cout << "Enabling debug privilege" << std::endl;
    HANDLE hToken = NULL;

    if (OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken) == NULL)
    {
       if (GetLastError() == ERROR_NO_TOKEN)
       {
           if (ImpersonateSelf(SecurityImpersonation) == FALSE)
           {
               std::cout << "ImpersonateSelf() failed: " << GetLastError() << std::endl;
               return false;
           }
           if (OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken) == FALSE)
           {
               std::cout << "OpenThreadToken() #2 failed: " << GetLastError() << std::endl;
               return false;
           }
       }
       else
       {
           std::cout << "OpenThreadToken() #1 failed: " << GetLastError() << std::endl;
           return false;
       }
    }

    LUID luid;
    if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid) == FALSE)
    {
       assert(false);
       std::cout << "Cannot lookup privilege: " << GetLastError() << std::endl;
       CloseHandle(hToken);
       return false;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid = luid;

    if (AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr) == FALSE)
    {
       assert(false);
       std::cout << "Cannot adjust privilege: " << GetLastError() << std::endl;
       CloseHandle(hToken);
       return false;
    }

    CloseHandle(hToken);
    return true;
}
