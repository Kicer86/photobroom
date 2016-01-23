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

    std::ostream &operator<<(std::ostream &o, const QSize &s)
    {
        return o << "[width: " << s.width () << "; height: " << s.height() << "]";
    }
}

QRect ModelIndexInfo::Postition::getRect() const
{
    const QRect result = valid && size.isValid()?
                         QRect(position, size):
                         QRect(0, 0, 0, 0);
                         
    return result;
}


void ModelIndexInfo::Postition::setRect(const QRect& rect)
{
    valid = rect.isValid();
    size = rect.size();
    position = rect.topLeft();
}


///////////////////////////////////////////////////////////////////////////////


void ModelIndexInfo::setPosition(const QPoint& p)
{
    position.valid = true;
    position.position = p;
}


void ModelIndexInfo::setSize(const QSize& s)
{
    position.size = s;
}


void ModelIndexInfo::setRect(const QRect& r)
{
    position.setRect(r);
    overallRect = QSize();          // not anymore valid
}


void ModelIndexInfo::setOverallSize(const QSize& r)
{
    overallRect = r;
}


QRect ModelIndexInfo::getOverallRect() const
{
    const QPoint pos    = getPosition();
    const QSize overall = getOverallSize();
    const QRect result = QRect(pos, overall);

    return result;
}


const QRect ModelIndexInfo::getRect() const
{
    return position.getRect();
}


const QSize& ModelIndexInfo::getOverallSize() const
{
    return overallRect;
}


const QPoint& ModelIndexInfo::getPosition() const
{
    return position.position;
}


const QSize& ModelIndexInfo::getSize() const
{
    return position.size;
}


void ModelIndexInfo::cleanRects()
{
    position = Postition();
    overallRect = QSize();
}


void ModelIndexInfo::markPositionInvalid()
{
    position.valid = false;
}


void ModelIndexInfo::markSizeInvalid()
{
    position.size = QSize();
}


bool ModelIndexInfo::isPositionValid() const
{
    return position.valid;
}


bool ModelIndexInfo::valid() const
{
    return isPositionValid() && isSizeValid() && overallRect.isValid();
}


bool ModelIndexInfo::isSizeValid() const
{
    return position.size.isValid() && position.size.isNull() == false;   // valid and not null
}


ModelIndexInfo::ModelIndexInfo(const QModelIndex& index): expanded(index.isValid() == false), position(), overallRect()
{
}


ModelIndexInfo::operator std::string() const
{
    std::stringstream result;
    result << getRect() << ", " << getOverallSize() << ", expanded: " << expanded;

    return result.str();
}

