/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef STATUS_HPP
#define STATUS_HPP

template<typename T, T ok>
class Status
{
    public:
        Status(): m_status(ok) {}
        Status(const T& status): m_status(status) {}
        Status(const Status &) = default;
        ~Status() {}

        const T& get() const
        {
            return m_status;
        }

        Status& operator=(const Status &) = default;

        Status& operator=(const T& v)
        {
            m_status = v;

            return *this;
        }

        bool operator==(const Status& other) const
        {
            return m_status == other.m_status;
        }

        operator bool() const
        {
            return m_status == ok;
        }

        operator const T&() const
        {
            return m_status;
        }

        bool operator!() const
        {
            return m_status != ok;
        }

        Status<T, ok>& operator&=(const T& v)     // perform AND operation between 'this' and 'v'. 'ok' status is equal to '1', any other to '0'
        {
            if (m_status == ok)                   // this is ok? Check 'v'
                m_status = v == ok? ok: v;

            return *this;
        }

        Status<T, ok>& operator&=(const Status<T, ok>& v)
        {
            *this &= v.m_status;

            return *this;
        }

        bool operator==(bool v) const
        {
            return (m_status == ok) == v;
        }

    private:
        T m_status;
};

#endif // STATUS_HPP
