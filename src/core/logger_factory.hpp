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

#ifndef LOGGERFACTORY_HPP
#define LOGGERFACTORY_HPP

#include <fstream>
#include <mutex>

#include "ilogger_factory.hpp"
#include "ilogger.hpp"

#include "core_export.h"

class CORE_EXPORT LoggerFactory final: public ILoggerFactory
{
    public:
        LoggerFactory(const QString &);
        LoggerFactory(const LoggerFactory &) = delete;
        virtual ~LoggerFactory() = default;

        LoggerFactory& operator=(const LoggerFactory &) = delete;
        void setLoggingLevel(ILogger::Severity);

        std::unique_ptr<ILogger> get(const QString& utility) const override;
        std::unique_ptr<ILogger> get(const QStringList& utility) const override;

    private:
        mutable std::ofstream m_logFile;
        ILogger::Severity m_loggingLevel;
        mutable std::mutex m_outputMutex;
};

#endif // LOGGERFACTORY_HPP
