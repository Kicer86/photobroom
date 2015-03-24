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

#ifndef LOGGERFACTORY_HPP
#define LOGGERFACTORY_HPP

#include "ilogger_factory.hpp"

#include "core_export.h"

class CORE_EXPORT LoggerFactory: public ILoggerFactory
{
    public:
        LoggerFactory();
        LoggerFactory(const LoggerFactory &) = delete;
        virtual ~LoggerFactory();

        LoggerFactory& operator=(const LoggerFactory &) = delete;

        void setPath(const QString &);

        std::unique_ptr<ILogger> get(const QString& utility) override;
        std::unique_ptr<ILogger> get(const std::vector<QString>& utility) override;

    private:
        QString m_path;
};

#endif // LOGGERFACTORY_HPP
