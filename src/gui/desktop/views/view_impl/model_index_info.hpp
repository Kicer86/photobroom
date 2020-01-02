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

#ifndef MODELINDEXINFO_HPP
#define MODELINDEXINFO_HPP

#include <QModelIndex>
#include <QRect>


struct ModelIndexInfo
{
        bool expanded;

        void setPosition(const QPoint &);
        void setSize(const QSize &);
        void setRect(const QRect& r);
        void setOverallSize(const QSize& r);

        const QRect& getRect() const;
        const QSize& getOverallSize() const;
        const QPoint getPosition() const;
        const QSize getSize() const;
        QRect getOverallRect() const;

        void cleanRects();
        void markPositionInvalid();
        void markSizeInvalid();
        void markRectInvalid();
        void markOverallSizeInvalid();

        bool isOverallSizeValid() const;
        bool isSizeValid() const;
        bool isPositionValid() const;
        bool valid() const;

        ModelIndexInfo(const QModelIndex &);

        operator std::string() const;

    private:
        QRect rect;
        QSize overallSize;
};

#endif // MODELINDEXINFO_H
