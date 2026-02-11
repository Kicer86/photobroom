/*
 * Photo Broom - photos management tool.
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

module;

#include <QDir>

#include "paths.hpp"

module broom.system;
import :filesystem;

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


QString FileSystem::getScriptsPath() const
{
    return read(Paths::scripts);
}


QString FileSystem::getDataPath() const
{
    return read(Paths::data);
}


QString FileSystem::commonPath(const QString& p1, const QString& p2 ) const
{
    int common_part = 0;

    const auto min = std::min( p1.length(), p2.length());
    const auto max = std::max( p1.length(), p2.length());

    for(int i = 0; i < min; i++)
    {
        if ( p1[i] == p2[i])
        {
            if (p1[i] == '/'  ||
                p1[i] == '\\' ||      // QDir::separator could be used here, but we may expect both types (from unit tests for example)
                i + 1 == max)         // is it end of path?
            {
                common_part = i + 1;
            }
        }
        else
            break;
    }

    const QString result = p1.left(common_part);

    return result;
}


QString FileSystem::read(const char* d) const
{
    QDir dir(d);
    dir.makeAbsolute();

    return dir.path();
}
