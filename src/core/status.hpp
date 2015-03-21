/*
 * Tiny status class.
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

        bool operator!() const
        {
            return m_status != ok;
        }

    private:
        T m_status;
};

#endif // STATUS_HPP
