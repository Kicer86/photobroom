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
#include <QString>

#include "ilogger.hpp"

#include "core_export.h"

class QIODevice;
class QString;

class CORE_EXPORT Logger: public ILogger
{
    public:
        Logger();
        Logger(const Logger& other) = delete;
        ~Logger();

        Logger& operator=(const Logger& other) = delete;

        void setPath(const QString &);
        void setLevel(Severity);

        virtual void log(const char* utility, Severity, const std::string& message) override;
        virtual void log(const std::vector<const char *>& utility, Severity, const std::string& message) override;

    private:
        QString m_basePath;
        Severity m_severity;
        std::map<QString, QIODevice *> m_files;

        QString getPath(const std::vector<const char *> &) const;
        QIODevice* getFile(const QString &);

        QString currentTime() const;
        void createPath(const QString &) const;
};

#endif // LOGGER_HPP
