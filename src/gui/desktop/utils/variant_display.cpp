/*
 * Tool for QVariant to readable string conversion.
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

#include "variant_display.hpp"

#include <cassert>

#include <QVariant>
#include <QLocale>
#include <QDate>
#include <QTime>


QString localize(const QVariant& v, const QLocale& l)
{
    const QVariant::Type type = v.type();
    QString result;

    switch(type)
    {
        case QVariant::Date:
        {
            QDate d = v.toDate();
            result = l.toString(d, QLocale::ShortFormat);
            break;
        }

        case QVariant::Time:
        {
            QTime t = v.toTime();
            result = l.toString(t, "hh:mm:ss");
            break;
        }

        case QVariant::String:
            result = v.toString();
            break;

        case QVariant::StringList:
            result = v.toStringList().join(", ");
            break;

        case QVariant::Int:
            result = QString::number(v.toInt());
            break;

        case QVariant::ULongLong:
            result = QString::number(v.toULongLong());
            break;

        default:
            assert(!"unexpected type");
            break;
    }

    return result;
}
