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

LoggerFactory::LoggerFactory(): m_path()
{

}


LoggerFactory::~LoggerFactory()
{

}


void LoggerFactory::setPath(const QString& path)
{
    m_path = path;
}


std::unique_ptr<ILogger> LoggerFactory::get(const QString& utility)
{
    return get( std::vector<QString>({utility}) );
}


std::unique_ptr<ILogger> LoggerFactory::get(const std::vector<QString>& utility)
{
    Logger* logger = new Logger(utility);
    logger->setPath(m_path);

    return std::unique_ptr<ILogger>(logger);
}
