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


std::vector<Tag::Types> BaseTags::getAll()
{
    const std::vector<Tag::Types> tags =
    {
        Tag::Types::Event,
        Tag::Types::Place,
        Tag::Types::Date,
        Tag::Types::Time,
        Tag::Types::Rating,
        Tag::Types::Category,
    };

    return tags;
}


QString BaseTags::getTr(Tag::Types tag)
{
    QString result;

    switch (tag)
    {
        case Tag::Types::Invalid:
            break;

        case Tag::Types::Event:
            result = tr("Event");
            break;

        case Tag::Types::Place:
            result = tr("Place");
            break;

        case Tag::Types::Date:
            result = tr("Date");
            break;

        case Tag::Types::Time:
            result = tr("Time");
            break;

        case Tag::Types::Rating:
            result = tr("Rating");
            break;

        case Tag::Types::Category:
            result = tr("Category");
            break;
    }

    return result;
}


QString BaseTags::getName(Tag::Types tag)
{
    QString result;

    switch (tag)
    {
        case Tag::Types::Invalid:
            break;

        case Tag::Types::Event:
            result = "Event";
            break;

        case Tag::Types::Place:
            result = "Place";
            break;

        case Tag::Types::Date:
            result = "Date";
            break;

        case Tag::Types::Time:
            result = "Time";
            break;

        case Tag::Types::Rating:
            result = "Rating";
            break;

        case Tag::Types::Category:
            result = "Category";
            break;
    }

    return result;
}


Tag::ValueType BaseTags::getType(Tag::Types tag)
{
    Tag::ValueType result = Tag::ValueType::Empty;

    switch (tag)
    {
        case Tag::Types::Invalid:
            break;

        case Tag::Types::Event:
        case Tag::Types::Place:
            result = Tag::ValueType::String;
            break;

        case Tag::Types::Date:
            result = Tag::ValueType::Date;
            break;

        case Tag::Types::Time:
            result = Tag::ValueType::Time;
            break;

        case Tag::Types::Rating:
            result = Tag::ValueType::Int;
            break;

        case Tag::Types::Category:
            result = Tag::ValueType::Color;
            break;
    }

    return result;
}

