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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <map>
#include <mutex>

#include <QStringList>

#include "ilogger.hpp"

#include "core_export.h"

class QIODevice;
class QString;

struct ILoggerFactory;


class CORE_EXPORT Logger: public ILogger
{
    public:
        Logger(std::mutex &, std::ostream &, const QString& utility, Severity, const ILoggerFactory *);
        Logger(std::mutex &, std::ostream &, const QStringList& utility, Severity, const ILoggerFactory *);
        Logger(const Logger& other) = delete;
        ~Logger() = default;

        Logger& operator=(const Logger& other) = delete;

        void log(Severity, const std::string& message) override;

        void info(const std::string &) override;
        void warning(const std::string &) override;
        void error(const std::string &) override;
        void debug(const std::string &) override;

        std::unique_ptr<ILogger> subLogger(const QString & sub_utility) override;

    private:
        const QStringList m_utility;
        Severity m_severity;
        std::ostream& m_file;
        std::mutex& m_outputMutex;
        const ILoggerFactory* m_loggerFactory;

        QString currentTime() const;
        QString currentDate() const;
        QString severity(Severity) const;
};

#endif // LOGGER_HPP
