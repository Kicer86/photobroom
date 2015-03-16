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


Logger::Logger(): m_basePath(""), m_severity(Severity::Warning), m_files()
{

}


Logger::~Logger()
{
    for(auto entry: m_files)
        delete entry.second;
}


void Logger::setPath(const QString& path)
{
    m_basePath = path;
}


void Logger::setLevel(ILogger::Severity severity)
{
    m_severity = severity;
}


void Logger::log(const char* utility, ILogger::Severity severity, const std::string& message)
{
    log( std::vector<const char *>({utility}), severity, message);
}


void Logger::log(const std::vector<const char *>& utility, ILogger::Severity, const std::string& message)
{
    assert(m_basePath.isEmpty() == false);

    const QString path = getPath(utility);
    QIODevice* file = getFile(path);
    
    QTextStream fileStream(file);

    fileStream << currentTime() << ": ";
    fileStream << message.c_str() << "\n";
}


QString Logger::getPath(const std::vector<const char *>& utility) const
{
    QString result(m_basePath);

    if (utility.empty() == false)
    {
        size_t i = 0;
        for(; i < utility.size(); i++)
            result = result + "/" + utility[i];

        result = result + ".log";
    }

    return result;
}


QIODevice* Logger::getFile(const QString& path)
{
    auto it = m_files.find(path);

    if (it == m_files.end())
    {
        createPath(path);
        QFile* file = new QFile(path);

        file->open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);
        file->write("\n");                                                        //Add new line everytime we open the file. Just making log files more clean.

        auto data = std::pair<QString, QIODevice *>(path, file);
        auto iit = m_files.insert(data);

        it= iit.first;
    }

    return it->second;
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
