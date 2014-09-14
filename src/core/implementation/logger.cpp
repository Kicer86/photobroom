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

Logger::Logger(): m_configuration(nullptr)
{

}


Logger::~Logger()
{
    for(auto entry: m_files)
        delete entry.second;
}


void Logger::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


void Logger::log(const char* utility, ILogger::Severity severity, const std::string& message)
{
    log( {utility}, severity, message);
}


void Logger::log(const std::vector<const char *>& utility, ILogger::Severity severity, const std::string& message)
{
    assert(m_configuration != nullptr);


}


std::string Logger::getPath(const std::vector<const char *>& utility)
{
    std::string result;

    if (utility.empty() == false)
    {
        size_t i = 0;
        for(; i < utility.size() - 1; i++)
            result = result + utility[i] + "/";

        result = result + utility[i] + ".log";
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
