/*
 * Filesystem utils.
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

#include "filesystem.hpp"

#include <QDir>

#include "paths.hpp"

QString FileSystem::getPluginsPath() const
{
    return read(Paths::plugins);
}


QString FileSystem::getTranslationsPath() const
{
    return read(Paths::translations);
}


QString FileSystem::getLibrariesPath() const
{
    return read(Paths::libraries);
}


QString FileSystem::commonPath(const QString& p1, const QString& p2 ) const
{
    int p = 0;
    for(int i = 0; i < std::min( p1.length(), p2.length()); i++)
    {
        if ( p1[i] == p2[i])
            p = i + 1;
        else
            break;
    }

    const QString result = p1.left(p);

    return result;
}


QString FileSystem::read(const char* d) const
{
    QDir dir(d);
    dir.makeAbsolute();

    return dir.path();
}
