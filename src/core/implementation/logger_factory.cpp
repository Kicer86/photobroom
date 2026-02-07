/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "logger_factory.hpp"

#include <iostream>

#include "logger.hpp"
#include "log_file_rotator.hpp"

LoggerFactory::LoggerFactory(const QString& path): m_logFile(), m_logingLevel(ILogger::Severity::Warning), m_outputMutex()
{
    const QString log_path = path + "/photo_broom.log";
    LogFileRotator().rotate(log_path);

    const std::string str_path = log_path.toStdString();

    m_logFile.open(str_path, std::ofstream::out | std::ofstream::app);

    if (!m_logFile.is_open())
        std::cerr << "Failed to open log file: " << str_path << std::endl;
}


void LoggerFactory::setLogingLevel(ILogger::Severity logingLevel)
{
    m_logingLevel = logingLevel;
}


std::unique_ptr<ILogger> LoggerFactory::get(const QString& utility) const
{
    return get( QStringList({utility}) );
}


std::unique_ptr<ILogger> LoggerFactory::get(const QStringList& utility) const
{
    return std::make_unique<Logger>(m_outputMutex, m_logFile, utility, m_logingLevel, this);
}
