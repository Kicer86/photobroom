/*
 * Model for view
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

#include "model_index_info.hpp"

#include <sstream>

namespace
{
    std::ostream &operator<<(std::ostream &o, const QRect &r)
    {
        return o << "[left: " << r.left () << "; top: " << r.top() << "; right: " << r.right() << "; bottom: " << r.bottom() << "]";
    }
}


void ModelIndexInfo::setRect(const QRect& r)
{
    rect = r;
    overallRect = QRect();          // not valid anymore
}


void ModelIndexInfo::setOverallRect(const QRect& r)
{
    overallRect = r;
}


const QRect& ModelIndexInfo::getRect() const
{
    return rect;
}


const QRect& ModelIndexInfo::getOverallRect() const
{
    return overallRect;
}


void ModelIndexInfo::cleanRects()
{
    rect = QRect();
    overallRect = QRect();
}


ModelIndexInfo::ModelIndexInfo(): expanded(false), rect(), overallRect()
{
}


ModelIndexInfo::operator std::string() const
{
    std::stringstream result;
    result << getRect() << ", " << getOverallRect() << ", expanded: " << expanded;
    
    return result.str();
}

