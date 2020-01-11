/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef PAINTERHELPERS_HPP
#define PAINTERHELPERS_HPP

#include <QPainter>
#include <QPen>

class PainterHelpers
{
    public:
        template<typename T>
        static void drawTextWithOutline(QPainter* painter, const QPoint& p, const T& text, const QPen& outlineColor)
        {
            const QPen currentPen = painter->pen();

            // draw outline by painting text 4 times (moved by one pixel in each direction from center)
            painter->setPen(outlineColor);
            painter->drawText(p + QPoint(-1,  0), text);
            painter->drawText(p + QPoint( 1,  0), text);
            painter->drawText(p + QPoint( 0, -1), text);
            painter->drawText(p + QPoint( 0,  1), text);

            // draw text
            painter->setPen(currentPen);
            painter->drawText(p, text);
        }
};

#endif // PAINTERHELPERS_HPP
