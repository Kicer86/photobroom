/*
 * QModelIndex position calculator
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

#ifndef POSITIONSCALCULATOR_HPP
#define POSITIONSCALCULATOR_HPP

#include <QRect>
#include <QPixmap>
#include <QItemSelection>

class QModelIndex;
class QAbstractItemModel;

class Data;

class PositionsCalculator
{
    public:
        PositionsCalculator(QAbstractItemModel *, Data *, int width);
        PositionsCalculator(const PositionsCalculator& other) = delete;
        ~PositionsCalculator();
        PositionsCalculator& operator=(const PositionsCalculator& other) = delete;

        void updateItems() const;

    private:
        QRect calcItemRect(const QModelIndex &) const;
        QPoint positionOfNext(const QModelIndex &) const;                             // calculate position of next item
        QPoint positionOfNextImage(const QModelIndex &) const;                        // calculate position of next item (image item)
        QPoint positionOfNextNode(const QModelIndex &) const;                         // calculate position of next item (node item)
        QPoint positionOfFirstChild(const QModelIndex &) const;                       // calculate position for first item

        int getitemWidth(const QModelIndex &) const;
        int getItemHeigth(const QModelIndex &) const;
        int getItemHeigth(const QModelIndex &, const QModelIndex &) const;            // max height for set of items
        QSize getItemSize(const QModelIndex &) const;
        QItemSelection selectRowFor(const QModelIndex &) const;

        void calcItemsOverallRect() const;                                            // size (rect) including all subitems

        QAbstractItemModel* m_model;
        Data* m_data;
        const int m_width;
};

#endif // POSITIONSCALCULATOR_HPP
