/*
 * Photo Broom - photos management tool.
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
#include <core/qt_operators.hpp>

#include "photo_types.hpp"
#include "database_export.h"


namespace Person
{
    using Id = Id<int, struct person_tag>;
    using Fingerprint = std::vector<double>;
}


class DATABASE_EXPORT PersonName final
{
    public:
        PersonName();
        explicit PersonName(const Person::Id &, const QString &);
        explicit PersonName(const QString &);
        PersonName(const PersonName &) = default;
        ~PersonName() = default;

        PersonName& operator=(const PersonName &) = default;
        auto operator<=>(const PersonName &) const = default;

        const Person::Id& id() const;
        const QString& name() const;

    private:
        Person::Id m_id;
        QString m_name;
};


class DATABASE_EXPORT PersonFingerprint
{
    public:
        using Id = ::Id<int, struct fingerprint_tag>;

        PersonFingerprint() {}
        explicit PersonFingerprint(const Person::Fingerprint& fingerprint): m_fingerprint(fingerprint) {}
        explicit PersonFingerprint(const Id& id, const Person::Fingerprint& fingerprint): m_fingerprint(fingerprint), m_id(id) {}

        auto operator<=>(const PersonFingerprint &) const = default;

        const Id& id() const { return m_id; }
        const Person::Fingerprint& fingerprint() const { return m_fingerprint; }

    private:
        Person::Fingerprint m_fingerprint;
        Id m_id;
};

/**
 * @brief Container for DB IDs of people related entries.
 */
class DATABASE_EXPORT PersonInfo
{
    public:
        using Id = ::Id<int, struct personinfo_tag>;

        Id id;
        Person::Id p_id;
        Photo::Id ph_id;
        PersonFingerprint::Id f_id;
        QRect rect;

        PersonInfo(): id(), p_id(), ph_id(), f_id(), rect()
        {
        }

        PersonInfo(const PersonInfo::Id& _id,
                   const Person::Id& _p_id,
                   const Photo::Id& _ph_id,
                   const PersonFingerprint::Id& _f_id,
                   const QRect& _rect):
            id(_id), p_id(_p_id), ph_id(_ph_id), f_id(_f_id), rect(_rect)
        {}

        PersonInfo(const Person::Id& _p_id,
                   const Photo::Id& _ph_id,
                   const PersonFingerprint::Id& _f_id,
                   const QRect& _rect):
            id(), p_id(_p_id), ph_id(_ph_id), f_id(_f_id), rect(_rect)
        {}

        PersonInfo(const PersonInfo &) = default;
        PersonInfo& operator=(const PersonInfo &) = default;
        auto operator<=>(const PersonInfo &) const = default;
};

/**
 *  @brief Container for person data (without DB IDs)
 */
struct PersonData
{
    QRect rect;
    Person::Fingerprint fingerprint;
    QString name;
};

/**
 * @brief Container for all people related data (DB IDs + values)
 */
class PersonFullInfo
{
public:
    PersonFullInfo() = default;
    PersonFullInfo(const PersonData& data)
        : position(data.rect)
        , fingerprint(data.fingerprint)
        , name(data.name)
    {}

    QRect position;
    PersonFingerprint fingerprint;
    PersonName name;
    PersonInfo::Id pi_id;

    auto operator<=>(const PersonFullInfo &) const = default;
};


Q_DECLARE_METATYPE(PersonName)

#endif
