/*
 * Factory for Loggers
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

#include "logger.hpp"

LoggerFactory::LoggerFactory(const QString& path): m_logingLevel(ILogger::Severity::Warning)
{
    const std::string str_path = path.toStdString();

    m_logFile.open(str_path + "/photo_broom.log", std::ofstream::out | std::ofstream::ate );
}


void LoggerFactory::setLogingLevel(ILogger::Severity logingLevel)
{
    m_logingLevel = logingLevel;
}


std::unique_ptr<ILogger> LoggerFactory::get(const QString& utility) const
{
    return get( std::vector<QString>({utility}) );
}


std::unique_ptr<ILogger> LoggerFactory::get(const std::vector<QString>& utility) const
{
    auto logger = std::make_unique<Logger>(m_logFile, utility, m_logingLevel);

    return logger;
}
