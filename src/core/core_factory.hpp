/*
 * Simple implementation of ICoreFactory
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

#ifndef COREFACTORY_HPP
#define COREFACTORY_HPP

#include "core_export.h"

#include "icore_factory.hpp"

/**
 * @todo write docs
 */
class CORE_EXPORT CoreFactory final: public ICoreFactory
{
public:
    /**
     * Default constructor
     */
    CoreFactory(ILoggerFactory *, IExifReaderFactory *, IConfiguration *, ITaskExecutor *);

    /**
     * Destructor
     */
    ~CoreFactory();

    /**
     * @todo write docs
     *
     * @return TODO
     */
    ILoggerFactory* getLoggerFactory() override;

    IExifReaderFactory* getExifReaderFactory() override;

    IConfiguration* getConfiguration() override;

    ITaskExecutor * getTaskExecutor() override;

private:
    ILoggerFactory* m_loggerFactory;
    IExifReaderFactory* m_exifReaderFactory;
    IConfiguration* m_configuration;
    ITaskExecutor* m_taskExecutor;
};

#endif // COREFACTORY_HPP
