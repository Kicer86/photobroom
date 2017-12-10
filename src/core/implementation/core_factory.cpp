/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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
 */


#include "core_factory.hpp"


CoreFactory::CoreFactory(ILoggerFactory* loggerFactory,
                         IExifReaderFactory* exifReader,
                         IConfiguration* configuration,
                         ITaskExecutor* taskExecutor
                        ):
    m_loggerFactory(loggerFactory),
    m_exifReaderFactory(exifReader),
    m_configuration(configuration),
    m_taskExecutor(taskExecutor)
{

}


CoreFactory::~CoreFactory()
{

}


ILoggerFactory* CoreFactory::getLoggerFactory()
{
    return m_loggerFactory;
}


IExifReaderFactory * CoreFactory::getExifReaderFactory()
{
    return m_exifReaderFactory;
}


IConfiguration * CoreFactory::getConfiguration()
{
    return m_configuration;
}


ITaskExecutor * CoreFactory::getTaskExecutor()
{
    return m_taskExecutor;
}

