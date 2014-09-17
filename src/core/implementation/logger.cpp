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
#include <fstream>
#include <chrono>
#include <iomanip>

#include <sstream>

#include <QString>

#include <configuration/iconfiguration.hpp>
#include <configuration/entrydata.hpp>
#include <configuration/constants.hpp>

template<typename R, typename T>
R lexical_cast(const T& value)
{
    std::stringstream str;
    R result;

    str << value;
    str >> result;

    return result;
}


namespace
{
    std::string put_time(const std::tm* t)
    {
        std::string result;

        result = result + lexical_cast<std::string>(t->tm_hour) + ":";
        result = result + lexical_cast<std::string>(t->tm_min) + ":";
        result = result + lexical_cast<std::string>(t->tm_sec);

        return result;
    }
}

Logger::Logger(): m_basePath(""), m_severity(Severity::Warning), m_files()
{

}


Logger::~Logger()
{
    for(auto entry: m_files)
        delete entry.second;
}


void Logger::setPath(const std::string& path)
{
    m_basePath = path;
}


void Logger::setLevel(ILogger::Severity severity)
{
    m_severity = severity;
}


void Logger::log(const char* utility, ILogger::Severity severity, const std::string& message)
{
    log( {utility}, severity, message);
}


void Logger::log(const std::vector<const char *>& utility, ILogger::Severity severity, const std::string& message)
{
    assert(m_basePath.empty() == false);

    const std::string path = getPath(utility);
    std::ostream* file = getFile(path);

    *file << currentTime() + ": ";
    *file << message << std::endl;
}


std::string Logger::getPath(const std::vector<const char *>& utility) const
{
    std::string result(m_basePath);

    if (utility.empty() == false)
    {
        size_t i = 0;
        for(; i < utility.size(); i++)
            result = result + "/" + utility[i];

        result = result + ".log";
    }

    return result;
}


std::ostream* Logger::getFile(const std::string& path)
{
    auto it = m_files.find(path);

    if (it == m_files.end())
    {
        std::fstream* file = new std::fstream;

        file->open(path, std::ios_base::app | std::ios_base::out);

        auto data = std::pair<std::string, std::ostream *>(path, file);
        auto iit = m_files.insert(data);

        it= iit.first;
    }

    return it->second;
}


std::string Logger::currentTime() const
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    return put_time(std::localtime(&now_c));
}


void Logger::createPath(const std::string& path) const
{

}
