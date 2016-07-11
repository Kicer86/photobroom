/*
 * Logger utility.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "logger.hpp"

#include <cassert>
#include <iostream>

#include <sstream>

#include <QString>
#include <QFile>
#include <QTime>
#include <QTextStream>
#include <QDir>
#include <QDebug>


Logger::Logger(std::ostream& stream, const std::vector<QString>& utility, Severity severity):
    m_utility(utility),
    m_severity(severity),
    m_file(stream)
{

}


Logger::Logger(std::ostream& stream, const QString& utility, Severity severity): Logger(stream, std::vector<QString>({ utility}), severity)
{

}


Logger::~Logger()
{

}


void Logger::log(ILogger::Severity severity, const std::string& message)
{
    const QString m = QString("%1: %2").arg(currentTime()).arg(message.c_str());

    if (severity <= m_severity)
    {
        m_file << m.toStdString() << "\n";
    }

    qDebug() << m;
}


void Logger::info(const std::string& msg)
{
    log(Severity::Info, msg);
}


void Logger::warning(const std::string& msg)
{
    log(Severity::Warning, msg);
}


void Logger::error(const std::string& msg)
{
    log(Severity::Error, msg);
}


void Logger::debug(const std::string& msg)
{
    log(Severity::Debug, msg);
}


QString Logger::currentTime() const
{
    QTime now = QTime::currentTime();
    const QString timeStr = now.toString("HH:mm:ss:zzz");

    return timeStr;
}
