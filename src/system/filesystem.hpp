/*
 * Filesystem utils
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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>

#include "system_export.h"

struct SYSTEM_EXPORT FileSystem
{
        QString getPluginsPath() const;
        QString getTranslationsPath() const;
        QString getLibrariesPath() const;
        QString getScriptsPath() const;
        QString getDataPath() const;

        QString commonPath(const QString &, const QString &) const;

    private:
        QString read(const char *) const;
};

#endif // FILESYSTEM_H
