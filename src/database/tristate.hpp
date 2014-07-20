/*
 * Tristate element.
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

#ifndef TRISTATE_HPP
#define TRISTATE_HPP

#include "database_export.h"

class DATABASE_EXPORT tristate
{
    public:
        enum Value
        {
            unknown,    // -
            zero,       // 0
            one,        // 1
        };

        tristate();
        tristate(bool);
        tristate(Value);
        tristate(const tristate& other);
        ~tristate();

        tristate& operator=(const tristate& other);
        bool operator==(const tristate& other);

        bool isUnknown() const;
        bool isZero() const;
        bool isOne() const;

    private:
        Value m_value;
};

#endif // TRIT_H
