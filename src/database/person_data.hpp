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
#include <core/universal_operators.hpp>

#include "photo_data.hpp"
#include "database_export.h"


namespace Person
{
    DATABASE_EXPORT extern const char Name[16];
    typedef Id<int, Name> Id;
}


class DATABASE_EXPORT PersonName final
{
    public:
        PersonName();
        PersonName(const Person::Id &, const QString &);
        PersonName(const QString &);
        PersonName(const PersonName &);
        ~PersonName() = default;

        PersonName& operator=(const PersonName &) = default;
        bool operator<(const PersonName& other) const
        {
            return isLess(*this, other,
                          &PersonName::m_id,
                          &PersonName::m_name);
        }

        const Person::Id& id() const;
        const QString& name() const;

    private:
        Person::Id m_id;
        QString m_name;
};

class DATABASE_EXPORT PersonInfo
{
        static const char Name[16];

    public:
        typedef ::Id<int, Name> Id;

        Id id;
        Person::Id p_id;
        Photo::Id ph_id;
        QRect rect;

        PersonInfo(): id(), p_id(), ph_id(), rect()
        {
        }

        PersonInfo(const PersonInfo::Id& _id,
                const Person::Id& _p_id,
                const Photo::Id& _ph_id,
                const QRect& _rect):
            id(_id), p_id(_p_id), ph_id(_ph_id), rect(_rect)
        {}

        PersonInfo(const PersonInfo &) = default;
        PersonInfo& operator=(const PersonInfo &) = default;

        bool operator==(const PersonInfo& other) const
        {
            return id == other.id       &&
                   p_id == other.p_id   &&
                   ph_id == other.ph_id &&
                   rect == other.rect;
        }
};

Q_DECLARE_METATYPE( PersonName )

#endif // PERSONDATA_HPP
