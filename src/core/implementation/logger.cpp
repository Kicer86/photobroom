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
#include <QDir>


Logger::Logger(std::mutex& output_mutex, std::ostream& stream, const QStringList& utility, Severity severity):
    m_utility(utility.join(":")),
    m_severity(severity),
    m_file(stream),
    m_outputMutex(output_mutex)
{

}


Logger::Logger(std::mutex& output_mutex, std::ostream& stream, const QString& utility, Severity severity): Logger(output_mutex, stream, QStringList({utility}), severity)
{

}


void Logger::log(ILogger::Severity sev, const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_outputMutex);

    const QString s = severity(sev);
    const QString m = QString("%1 %2 [%3][%4]: %5")
                        .arg(currentDate())
                        .arg(currentTime())
                        .arg(s)
                        .arg(m_utility)
                        .arg(message.c_str());

    const std::string message_str = m.toStdString();

    if (sev <= m_severity)
        m_file << message_str << "\n";

    std::cout << message_str << std::endl;
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


QString Logger::currentDate() const
{
    QDate now = QDate::currentDate();
    const QString dateStr = now.toString("yyyy-MM-dd");

    return dateStr;
}


QString Logger::severity(ILogger::Severity s) const
{
    QString result;

    switch(s)
    {
        case Severity::Error:   result = "E"; break;
        case Severity::Warning: result = "W"; break;
        case Severity::Info:    result = "I"; break;
        case Severity::Debug:   result = "D"; break;
    }

    return result;
}
