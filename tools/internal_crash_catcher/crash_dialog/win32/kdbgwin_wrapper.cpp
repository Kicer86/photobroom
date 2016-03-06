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

#include <QDebug>


KDbgWinWrapper::KDbgWinWrapper(const QString& path):
    m_kdbgwin_path(path),
    m_kdbgwin(),
    m_callback(),
    m_pid(-1),
    m_tid(-1),
    m_exec()
{
    auto errorSignal = static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::error);
    auto finishedSignal = static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished);

    connect(&m_kdbgwin, errorSignal, this, &KDbgWinWrapper::kdbgwinError);
    connect(&m_kdbgwin, finishedSignal, this, &KDbgWinWrapper::kdbgwinFinished);

    m_kdbgwin.setProcessChannelMode(QProcess::MergedChannels);
}


KDbgWinWrapper::~KDbgWinWrapper()
{

}


bool KDbgWinWrapper::init(qint64 pid, qint64 tid, const QString &exec)
{
    m_pid = pid;
    m_tid = tid;
    m_exec = exec;

    return true;
}


void KDbgWinWrapper::requestBacktrace(const std::function<void (const std::vector<QString> &)>& callback)
{
    assert(m_pid > 0);
    assert(m_tid >= 0);

    m_callback = callback;

    QStringList args;
    args << QString().setNum(m_pid);
    args << QString().setNum(m_tid);

    m_kdbgwin.start(m_kdbgwin_path, args);
}


const QString& KDbgWinWrapper::exec() const
{
    return m_exec;
}


void KDbgWinWrapper::kdbgwinError(QProcess::ProcessError error)
{
    qCritical().noquote() << "KDbgWin error:" << error;
}


void KDbgWinWrapper::kdbgwinFinished(int, QProcess::ExitStatus)
{
    std::vector<QString> backtrace;

    for(;;)
    {
        const QByteArray line = m_kdbgwin.readLine();

        if (line.isEmpty())
            break;

        const QString lineStr = line.constData();
        backtrace.push_back( lineStr.trimmed() );
    }

    m_callback(backtrace);
}
