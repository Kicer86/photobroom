/*
 * Photo Broom - photos management tool.
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

#ifndef TIMEGUARDIAN_H
#define TIMEGUARDIAN_H

#include <memory>
#include <string>

#include "core_export.h"

class string;

#ifndef NDEBUG
    #define TIME_GUARDIAN(t,l,m) TimeGuardian _guardian;  \
        _guardian.setLimit(l,m); \
        _guardian.setTitle(t);   \
        _guardian.begin();
#else
    #define TIME_GUARDIAN(t,l,m) ;
#endif

class CORE_EXPORT TimeGuardian
{
    public:
        TimeGuardian();
        TimeGuardian(const TimeGuardian& other) = delete;
        ~TimeGuardian();

        void setLimit(unsigned int ms, const std::string& message);
        void setTitle(const std::string &);
        void begin();

        TimeGuardian& operator=(const TimeGuardian& other) = delete;

    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif // TIMEGUARDIAN_H
