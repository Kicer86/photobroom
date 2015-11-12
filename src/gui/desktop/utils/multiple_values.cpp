/*
 * Type used by tags editor for tags with nonmatching values
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

#include <QVariant>

#include "multiple_values.hpp"


MultipleValues::MultipleValues(): m_userType(QVariant::Invalid)
{

}


MultipleValues::MultipleValues(int userType): m_userType(userType)
{

}


MultipleValues::~MultipleValues()
{

}


int MultipleValues::userType() const
{
    return m_userType;
}

