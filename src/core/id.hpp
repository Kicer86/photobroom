/*
 * Generic Id class
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

template<typename T>
class Id
{
    public:
        typedef T type;

        Id(): m_value(-1), m_valid(false)
        {

        }

        explicit Id(const T& id): m_value(id), m_valid(true)
        {

        }

        Id(const Id &) = default;

        Id& operator=(const Id &) = default;

        operator T() const
        {
            assert(m_valid);
            return m_value;
        }

        bool operator!() const
        {
            return !m_valid;
        }

        bool valid() const
        {
            return m_valid;
        }

        T value() const
        {
            return m_value;
        }

    private:
        T m_value;
        bool m_valid;
};

#endif // ID_HPP
