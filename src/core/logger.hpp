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

#include <QStringList>

#include "ilogger.hpp"

#include "core_export.h"

class QIODevice;
class QString;

class CORE_EXPORT Logger: public ILogger
{
    public:
        Logger(std::ostream &, const QString& utility, Severity = Severity::Warning);
        Logger(std::ostream &, const QStringList& utility, Severity = Severity::Warning);
        Logger(const Logger& other) = delete;
        ~Logger() = default;

        Logger& operator=(const Logger& other) = delete;

        void log(Severity, const QString& message) override;

        void info(const QString &) override;
        void warning(const QString &) override;
        void error(const QString &) override;
        void debug(const QString &) override;

    private:
        const QString m_utility;
        Severity m_severity;
        std::ostream& m_file;

        QString currentTime() const;
        QString currentDate() const;
        QString severity(Severity) const;
};

#endif // LOGGER_HPP
