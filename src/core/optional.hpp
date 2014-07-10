/*
 * Container for optional data.
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

#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

template<typename T>
class Optional
{
    public:
        Optional(): m_data(), m_present(false) {}
        Optional(const Optional<T>& other): m_data(other.m_data), m_present(other.m_present) {}
        Optional(const T& data): m_data(data), m_present(true) {}

        Optional<T>& operator=(const Optional<T>& other)
        {
            m_data = other.m_data;
            m_present = other.m_present;

            return *this;
        }

        Optional<T>& operator=(const T& data)
        {
            m_data = data;
            m_present = true;

            return *this;
        }

        const T* operator->() const
        {
            return &m_data;
        }

        T* operator->()
        {
            return &m_data;
        }

        const T& operator*() const
        {
            return m_data;
        }

        bool operator!() const
        {
            return !m_present;
        }

        operator bool() const
        {
            return m_present;
        }

        bool is_initialized() const
        {
            return m_present;
        }

    private:
        T m_data;
        bool m_present;
};

#endif // OPTIONAL_HPP
