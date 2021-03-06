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

#include "photo_data.hpp"
#include "database_export.h"


namespace Person
{
    using Id = Id<int, struct person_tag>;
    typedef std::vector<double> Fingerprint;
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
            return std::tie(m_id, m_name) < std::tie(other.m_id, other.m_name);
        }

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
        PersonFingerprint(const Person::Fingerprint& fingerprint): m_fingerprint(fingerprint) {}
        PersonFingerprint(const Id& id, const Person::Fingerprint& fingerprint): m_fingerprint(fingerprint), m_id(id) {}

        const Id& id() const { return m_id; }
        const Person::Fingerprint& fingerprint() const { return m_fingerprint; }

    private:
        Person::Fingerprint m_fingerprint;
        Id m_id;
};


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

        bool operator==(const PersonInfo& other) const
        {
            return id == other.id       &&
                   p_id == other.p_id   &&
                   ph_id == other.ph_id &&
                   f_id == other.f_id   &&
                   rect == other.rect;
        }
};

Q_DECLARE_METATYPE( PersonName )

#endif // PERSONDATA_HPP
