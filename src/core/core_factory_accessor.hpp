/*
 * Photo Broom - photos management tool.
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


#ifndef COREFACTORYACCESSOR_HPP
#define COREFACTORYACCESSOR_HPP

#include "core_export.h"

#include "icore_factory_accessor.hpp"


class CORE_EXPORT CoreFactoryAccessor final: public ICoreFactoryAccessor
{
    public:
        CoreFactoryAccessor(ILoggerFactory *,
                            IExifReaderFactory *,
                            IConfiguration *,
                            ITaskExecutor *,
                            IFeaturesManager *
                           );
        ~CoreFactoryAccessor();

        ILoggerFactory* getLoggerFactory() override;
        IExifReaderFactory* getExifReaderFactory() override;
        IConfiguration* getConfiguration() override;
        ITaskExecutor * getTaskExecutor() override;
        IFeaturesManager * getFeaturesManager() override;

    private:
        ILoggerFactory* m_loggerFactory;
        IExifReaderFactory* m_exifReaderFactory;
        IConfiguration* m_configuration;
        ITaskExecutor* m_taskExecutor;
        IFeaturesManager* m_featuresManager;
};

#endif // COREFACTORYACCESSOR_HPP
