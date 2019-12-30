/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  <copyright holder> <email>
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

#ifndef ICONSLOADER_HPP
#define ICONSLOADER_HPP

#include <QStyle>


// knowledge base:
//
// http://mithatkonar.com/wiki/doku.php/qt/icons
// http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
// http://doc.qt.io/qt-5/qicon.html#fromTheme
// http://doc.qt.io/qt-5/qstyle.html#StandardPixmap-enum


class IconsLoader
{
    public:
        IconsLoader();
        IconsLoader(const IconsLoader &) = delete;
        ~IconsLoader();

        IconsLoader& operator=(const IconsLoader &) = delete;

        enum class Icon
        {
            New,
            Open,
            Quit,
            Close,
            Settings,
            Help,
            About,
            AboutQt,
        };

        QIcon getIcon(Icon) const;

    private:
        const std::map<Icon, std::pair<const char *, QStyle::StandardPixmap>> m_icons =
        {
            { Icon::New,      {"document-new",        QStyle::SP_FileIcon}                 },
            { Icon::Open,     {"document-open",       QStyle::SP_DirOpenIcon}              },
            { Icon::Quit,     {"application-exit",    QStyle::SP_CustomBase}               },
            { Icon::Close,    {"window-close",        QStyle::SP_DialogCloseButton}        },
            { Icon::Settings, {"applications-system", QStyle::SP_CustomBase}               },
            { Icon::Help,     {"help-contents",       QStyle::SP_DialogHelpButton}         },
            { Icon::About,    {"help-about",          QStyle::SP_CustomBase}               },
            { Icon::AboutQt,  {"help-about",          QStyle::SP_CustomBase}               },
        };
};

#endif // ICONSLOADER_HPP
