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

#include "model_index_info.hpp"

#include <sstream>

namespace
{
    std::ostream &operator<<(std::ostream &o, const QRect &r)
    {
        return o << "[left: " << r.left () << "; top: " << r.top() << "; right: " << r.right() << "; bottom: " << r.bottom() << "]";
    }

    std::ostream &operator<<(std::ostream &o, const QSize &s)
    {
        return o << "[width: " << s.width () << "; height: " << s.height() << "]";
    }

    const QPoint invalidPosition(-1, -1);
    const QSize  invalidSize(0, 0);
    const QRect  invalidRect(invalidPosition, invalidSize);
}


///////////////////////////////////////////////////////////////////////////////


void ModelIndexInfo::setPosition(const QPoint& p)
{
    rect.moveTo(p);
}


void ModelIndexInfo::setSize(const QSize& s)
{
    rect.setSize(s);
}


void ModelIndexInfo::setRect(const QRect& r)
{
    rect = r;
    overallSize = QSize();          // not anymore valid
}


void ModelIndexInfo::setOverallSize(const QSize& r)
{
    overallSize = r;
}


const QRect& ModelIndexInfo::getRect() const
{
    return rect;
}


const QSize& ModelIndexInfo::getOverallSize() const
{
    return overallSize;
}


const QPoint ModelIndexInfo::getPosition() const
{
    return rect.topLeft();
}


const QSize ModelIndexInfo::getSize() const
{
    return rect.size();
}


QRect ModelIndexInfo::getOverallRect() const
{
    const QPoint position = getPosition();
    const QRect result = QRect(position, overallSize);

    return result;
}


void ModelIndexInfo::cleanRects()
{
    markPositionInvalid();
    markSizeInvalid();

    overallSize = invalidSize;
}


void ModelIndexInfo::markPositionInvalid()
{
    rect.moveTo(invalidPosition);
}


void ModelIndexInfo::markSizeInvalid()
{
   rect.setSize(invalidSize);
}


void ModelIndexInfo::markRectInvalid()
{
    rect = invalidRect;
}


void ModelIndexInfo::markOverallSizeInvalid()
{
    overallSize = invalidSize;
}



bool ModelIndexInfo::isPositionValid() const
{
    return rect.topLeft() != invalidPosition;
}


bool ModelIndexInfo::valid() const
{
    return isPositionValid() && isSizeValid() && overallSize.isValid();
}


bool ModelIndexInfo::isOverallSizeValid() const
{
    return overallSize != invalidSize;
}


bool ModelIndexInfo::isSizeValid() const
{
    return rect.size() != invalidSize;
}


ModelIndexInfo::ModelIndexInfo(const QModelIndex& index): expanded(index.isValid() == false), rect(invalidRect), overallSize(invalidSize)
{
}


ModelIndexInfo::operator std::string() const
{
    std::stringstream result;
    result << getRect() << ", " << getOverallSize() << ", expanded: " << expanded;

    return result.str();
}
