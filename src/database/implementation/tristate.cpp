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

#include "tristate.hpp"

tristate::tristate(): m_value(unknown)
{

}


tristate::tristate(bool v): m_value(v? one: zero)
{

}


tristate::tristate(tristate::Value v): m_value(v)
{

}


tristate::tristate(const tristate& other): m_value(other.m_value)
{

}


tristate::~tristate()
{

}


tristate& tristate::operator=(const tristate& other)
{
    m_value = other.m_value;
}


bool tristate::operator==(const tristate& other)
{
    return m_value == other.m_value;
}


bool tristate::isUnknown() const
{
    return m_value == unknown;
}


bool tristate::isZero() const
{
    return m_value == zero;
}


bool tristate::isOne() const
{
    return m_value == one;
}
