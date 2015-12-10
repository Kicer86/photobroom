/*
 * Default (system) icons loader.
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

#include "icons_loader.hpp"

#include <QApplication>


IconsLoader::IconsLoader()
{

}


IconsLoader::~IconsLoader()
{

}


QIcon IconsLoader::getIcon(QStyle::StandardPixmap icon) const
{
    QIcon result;
    QStyle* style = QApplication::style();

    switch (icon)
    {
        case QStyle::SP_FileIcon:
            if (QIcon::hasThemeIcon("document-new"))
                result = QIcon::fromTheme("document-new");
            else
                result = style->standardIcon(icon);
            break;

        case QStyle::SP_DirOpenIcon:
            if (QIcon::hasThemeIcon("document-open"))
                result = QIcon::fromTheme("document-open");
            else
                result = style->standardIcon(icon);
            break;

        case QStyle::SP_DialogCloseButton:
            if (QIcon::hasThemeIcon("application-exit"))
                result = QIcon::fromTheme("application-exit");
            else
                result = style->standardIcon(icon);
            break;

        default:
            break;
    }

    return result;
}
