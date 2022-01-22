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

#ifndef VARIANTDISPLAY_HPP
#define VARIANTDISPLAY_HPP

#include <QLocale>
#include <QString>

#include <core/tags_utils.hpp>

class QVariant;


class Variant: public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE static QString localize(const QVariant &, const QLocale & = QLocale());
    Q_INVOKABLE static QString localize(Tag::Types type, const QVariant &, const QLocale & = QLocale());
};

#endif // VARIANTDISPLAY_HPP
