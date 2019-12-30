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

#ifndef VERSION_HPP
#define VERSION_HPP

#include <regex>

#include <QString>


class Version
{
    public:
        Version();
        Version(const Version &) = default;
        Version(int, int, int);
        ~Version();

        Version& operator=(const Version &) = default;

        bool operator==(const Version &) const;
        bool operator<(const Version &) const;
        bool operator>(const Version &) const;

        static Version fromTagName(const QString &);       // expecting vX.Y.Z format
        static Version fromString(const char *);           // expecting X.Y.Z format

    private:
        int m_major;
        int m_minor;
        int m_patch;

        static Version build(const std::string&, const std::regex &);
};

#endif // VERSION_HPP
