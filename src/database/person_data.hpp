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

#include <QString>
#include <QRect>

#include <core/id.hpp>

#include "database_export.h"

namespace Person
{
    typedef Id<int> Id;
}

class DATABASE_EXPORT PersonData final
{
    public:
        PersonData(const Person::Id &, const QString &, const QString &);
        PersonData(const PersonData &);
        ~PersonData() = default;

        const Person::Id& id() const;
        const QString& name() const;
        const QString& path() const;

    private:
        const Person::Id m_id;
        const QString m_name;
        const QString m_path;
};

struct PersonLocation
{
    const Person::Id id;
    const QRect location;

    PersonLocation(const Person::Id& i, const QRect& l): id(i), location(l) {}
};

#endif // PERSONDATA_HPP
