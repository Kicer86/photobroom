/*
 * Command line parser
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

#include "command_line_parser.hpp"

#include <QCommandLineParser>

CommandLineParser::CommandLineParser(const QCoreApplication& app): m_error(), m_exec(), m_pid(0), m_tid(0)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("CrashDialog is an application which attaches to crashed program and shows backtrace.");
    parser.addHelpOption();
    parser.addVersionOption();

    // Pid
    QCommandLineOption pidOption(QStringList() << "p" << "pid",
                                 QCoreApplication::translate("CommandLineParser", "Pid of crashed program."),
                                 QCoreApplication::translate("CommandLineParser", "pid"));
    parser.addOption(pidOption);

    // Tid
    QCommandLineOption tidOption(QStringList() << "t" << "tid",
                                 QCoreApplication::translate("CommandLineParser", "Id of thread causing crash."),
                                 QCoreApplication::translate("CommandLineParser", "thread id"));
    parser.addOption(tidOption);

    // exec path
    QCommandLineOption execOption(QStringList() << "e" << "exec",
                                  QCoreApplication::translate("CommandLineParser", "path to executable file."),
                                  QCoreApplication::translate("CommandLineParser", "exec path"));
    parser.addOption(execOption);

    // parse
    parser.process(app);

    const QString pid = parser.value(pidOption);
    const QString tid = parser.value(tidOption);
    const QString exec = parser.value(execOption);

    if (pid.isEmpty())
        m_error = "--pid option is required.";
    else if (tid.isEmpty())
        m_error = "--tid option is required.";
    else if (exec.isEmpty())
        m_error = "--exec option is required.";
    else
    {
        m_pid = pid.toLong();
        m_tid = tid.toLong();
        m_exec = exec;
    }
}


CommandLineParser::~CommandLineParser()
{

}


bool CommandLineParser::error() const
{
    return m_error.isEmpty() == false;
}


const QString& CommandLineParser::errorMsg() const
{
    return m_error;
}


qint64 CommandLineParser::pid() const
{
    return m_pid;
}


qint64 CommandLineParser::tid() const
{
    return m_tid;
}


const QString& CommandLineParser::exec() const
{
    return m_exec;
}
