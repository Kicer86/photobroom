/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef ID_HPP
#define ID_HPP

#include <cassert>
#include <QVariant>


template<typename T, typename Tag>
class Id
{
    public:
        typedef T type;

        Id()
        {

        }

        explicit Id(const T& id): m_value(id), m_valid(true)
        {

        }

        explicit Id(const QVariant& variant)
        {
            m_valid = variant.canConvert<T>();

            if (m_valid)
                m_value = variant.value<T>();
        }

        Id(const Id &) = default;

        Id& operator=(const Id &) = default;

        Id& operator=(const T& id)
        {
            m_value = id;
            m_valid = true;

            return *this;
        }

        operator T() const
        {
            assert(m_valid);
            return m_value;
        }

        bool operator!() const
        {
            return !m_valid;
        }

        bool operator!=(const Id& other) const
        {
            return m_valid != other.m_valid || m_value != other.m_value;
        }

        auto operator<=>(const Id &) const = default;

        bool valid() const
        {
            return m_valid;
        }

        T value() const
        {
            return m_value;
        }

        void swap(Id& other)
        {
            std::swap(m_value, other.m_value);
            std::swap(m_valid, other.m_valid);
        }

        void invalidate()
        {
            swap(*this, {});
        }

    private:
        T m_value = {};
        bool m_valid = false;
};

#endif // ID_HPP
