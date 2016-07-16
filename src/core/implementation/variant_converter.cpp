/*
 * Tool for TagValue to QString conversion.
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

#include "variant_converter.hpp"

#include <cassert>

#include <QDate>
#include <QTime>

#include <core/tag.hpp>

VariantConverter::VariantConverter()
{

}


VariantConverter::~VariantConverter()
{

}


QString VariantConverter::operator()(const QVariant& v) const
{

    const QVariant::Type type = v.type();
    QString result;

    switch(type)
    {
        default:
            assert(!"unknown type");

        case QVariant::String:
            result = v.toString();
            break;

        case QVariant::Date:
            result = v.toDate().toString("yyyy.MM.dd");
            break;

        case QVariant::Time:
            result = v.toTime().toString("HH:mm:ss");
            break;

        case QVariant::StringList:
            result = v.toStringList().join(";");
            break;
    };

    return result;
}


QVariant VariantConverter::operator()(const TagNameInfo::Type& type, const QString& tag_value) const
{
    QVariant result;

    switch(type)
    {
        case TagNameInfo::Invalid:
        case TagNameInfo::Text:
            result = tag_value;
            break;

        case TagNameInfo::Date:
            result = QDate::fromString(tag_value, "yyyy.MM.dd");
            break;

        case TagNameInfo::Time:
            result = QTime::fromString(tag_value, "HH:mm:ss");
            break;

        case TagNameInfo::List:
        {
            const QStringList list = tag_value.split(";");
            result = list;
        }
    };

    return result;
}
