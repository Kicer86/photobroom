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

#include "variant_display.hpp"

#include <cassert>

#include <QColor>
#include <QDate>
#include <QTime>
#include <QVariant>


QString Variant::localize(const QVariant& v, const QLocale& l)
{
    const int type = v.typeId();
    QString result;

    switch(type)
    {
        case QMetaType::Type::QDate:
            result = localize(Tag::Date, v, l);
            break;

        case QMetaType::Type::QTime:
            result = localize(Tag::Time, v, l);
            break;

        case QMetaType::Type::QString:
            result = v.toString();
            break;

        case QMetaType::Type::QStringList:
            result = v.toStringList().join(", ");
            break;

        case QMetaType::Type::Int:
            result = QString::number(v.toInt());
            break;

        case QMetaType::Type::ULongLong:
            result = QString::number(v.toULongLong());
            break;

        case QMetaType::Type::QColor:
            result = localize(Tag::Category, v, l);
            break;

        default:
            assert(!"unexpected type");
            break;
    }

    return result;
}


QString Variant::localize(Tag::Types type, const QVariant& v, const QLocale& l)
{
    QString result;

    if (v.isValid())
        switch(type)
        {
            case Tag::Date:
            {
                QDate d = v.toDate();
                result = l.toString(d, QLocale::ShortFormat);
                break;
            }

            case Tag::Time:
            {
                QTime t = v.toTime();
                result = l.toString(t, "hh:mm:ss");
                break;
            }

            case Tag::Event:
            case Tag::Place:
                result = v.toString();
                break;

            case Tag::Category:
                result = QString::number(v.value<QColor>().rgba64());
                break;

            case Tag::Rating:
                result = QString::number(v.toInt());
                break;

            case Tag::Invalid:
                break;
        }

    return result;
}
