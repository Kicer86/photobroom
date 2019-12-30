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

#include "base_tags.hpp"

#include <QObject>


std::vector<TagTypes> BaseTags::getAll()
{
    const std::vector<TagTypes> tags =
    {
        TagTypes::Event,
        TagTypes::Place,
        TagTypes::Date,
        TagTypes::Time,
        TagTypes::Rating,
        TagTypes::Category,
    };

    return tags;
}


QString BaseTags::getTr(TagTypes tag)
{
    QString result;

    switch (tag)
    {
        case TagTypes::Invalid:
            break;

        case TagTypes::Event:
            result = tr("Event");
            break;

        case TagTypes::Place:
            result = tr("Place");
            break;

        case TagTypes::Date:
            result = tr("Date");
            break;

        case TagTypes::Time:
            result = tr("Time");
            break;

        case TagTypes::Rating:
            result = tr("Rating");
            break;

        case TagTypes::Category:
            result = tr("Category");
            break;
    }

    return result;
}


QString BaseTags::getName(TagTypes tag)
{
    QString result;

    switch (tag)
    {
        case TagTypes::Invalid:
            break;

        case TagTypes::Event:
            result = "Event";
            break;

        case TagTypes::Place:
            result = "Place";
            break;

        case TagTypes::Date:
            result = "Date";
            break;

        case TagTypes::Time:
            result = "Time";
            break;

        case TagTypes::Rating:
            result = "Rating";
            break;

        case TagTypes::Category:
            result = "Category";
            break;
    }

    return result;
}


Tag::ValueType BaseTags::getType(TagTypes tag)
{
    Tag::ValueType result = Tag::ValueType::Empty;

    switch (tag)
    {
        case TagTypes::Invalid:
            break;

        case TagTypes::Event:
        case TagTypes::Place:
            result = Tag::ValueType::String;
            break;

        case TagTypes::Date:
            result = Tag::ValueType::Date;
            break;

        case TagTypes::Time:
            result = Tag::ValueType::Time;
            break;

        case TagTypes::Rating:
            result = Tag::ValueType::Int;
            break;

        case TagTypes::Category:
            result = Tag::ValueType::Color;
            break;
    }

    return result;
}

