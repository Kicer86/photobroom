/*
 * struct with information about people.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef PERSONDATA_HPP
#define PERSONDATA_HPP

#include <QMetaType>
#include <QString>
#include <QRect>

#include <core/id.hpp>

#include "photo_data.hpp"
#include "database_export.h"

namespace Person
{
    extern const char Name[];
    typedef Id<int, Name> Id;
}

namespace Face
{
    extern const char Name[];
    typedef Id<int, Name> Id;
}

class DATABASE_EXPORT PersonData final
{
    public:
        PersonData();
        PersonData(const Person::Id &, const QString &);
        PersonData(const PersonData &);
        ~PersonData() = default;

        PersonData& operator=(const PersonData &) = default;

        const Person::Id& id() const;
        const QString& name() const;

    private:
        Person::Id m_id;
        QString m_name;
};

struct PersonLocation
{
    const Person::Id id;
    const QRect location;

    PersonLocation(const Person::Id& i, const QRect& l): id(i), location(l) {}
};


struct FaceData
{
    Face::Id id;
    Photo::Id ph_id;
    QRect rect;

    FaceData(): id(), ph_id(), rect()
    {
    }

    FaceData(const Face::Id& _id, const Photo::Id& _ph_id, const QRect& _rect):
        id(_id), ph_id(_ph_id), rect(_rect)
    {}

    FaceData(const FaceData &) = default;
    FaceData& operator=(const FaceData &) = default;

    bool operator==(const FaceData& other) const
    {
        return id == other.id &&
               ph_id == other.ph_id &&
               rect == other.rect;
    }
};

Q_DECLARE_METATYPE(PersonData)

#endif // PERSONDATA_HPP
