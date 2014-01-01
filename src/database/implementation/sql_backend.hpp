/*
 * Base for SQL-based backends
 * This class is meant to be included to each project using it.
 *
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

#ifndef ASQLBACKEND_H
#define ASQLBACKEND_H

#include "idatabase.hpp"

class ASqlBackend: public Database::IBackend
{
    public:
        ASqlBackend();
        ASqlBackend(const ASqlBackend& other) = delete;
        virtual ~ASqlBackend();

        ASqlBackend& operator=(const ASqlBackend& other) = delete;
        bool operator==(const ASqlBackend& other) = delete;


};

#endif // ASQLBACKEND_H

