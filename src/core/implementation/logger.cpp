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


Logger::Logger(const QString& path, const std::vector<QString>& utility):
    m_utility(utility),
    m_path(getPath(path)),
    m_severity(Severity::Warning),
    m_file(nullptr)
{

}


Logger::Logger(const QString& path, const QString& utility): Logger(path, std::vector<QString>({ utility}) )
{

}


Logger::~Logger()
{
    delete m_file;
}


void Logger::setPath(const QString& path)
{
    getPath(path);
    prepareFile();
}


void Logger::setLevel(ILogger::Severity severity)
{
    m_severity = severity;
}


void Logger::log(ILogger::Severity, const std::string& message)
{
    assert(m_path.isEmpty() == false);
    
    QTextStream fileStream(m_file);

    fileStream << currentTime() << ": ";
    fileStream << message.c_str() << "\n";
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


QString Logger::getPath(const QString& path) const
{
    QString result(path);

    if (m_utility.empty() == false)
    {
        for(size_t i = 0; i < m_utility.size(); i++)
            result = result + "/" + m_utility[i];

        result += ".log";
    }

    return result;
}


void Logger::prepareFile()
{
    assert(m_file == nullptr);

    createPath(m_path);
    m_file = new QFile(m_path);

    m_file->open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);
    m_file->write("\n");                                                        //Add new line everytime we open the file. Just making log files more clean.
}


QString Logger::currentTime() const
{
    QTime now = QTime::currentTime();
    const QString timeStr = now.toString("HH:mm:ss:zzz");
    
    return timeStr;
}


void Logger::createPath(const QString& path) const
{
    const QFileInfo fileInfo(path);
    const QString absPath = fileInfo.absolutePath();
    QDir().mkpath(absPath);
}
