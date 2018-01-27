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

#include "data.hpp"

class QModelIndex;
class QAbstractItemModel;

class PositionsCalculator
{
    public:
        PositionsCalculator(Data *, int width);
        PositionsCalculator(const PositionsCalculator& other) = delete;
        ~PositionsCalculator();
        PositionsCalculator& operator=(const PositionsCalculator& other) = delete;

        void updateItems() const;

    private:
        QRect calcItemRect(Data::ModelIndexInfoSet::Model::const_level_iterator) const;
        QSize calcItemSize(Data::ModelIndexInfoSet::Model::const_iterator) const;
        QPoint calcItemPosition(Data::ModelIndexInfoSet::Model::const_level_iterator) const;
        QPoint calcPositionOfNext(Data::ModelIndexInfoSet::Model::const_iterator) const;              // calculate position of next item
        QPoint calcPositionOfNextImage(const QModelIndex &) const;                                    // calculate position of next item (image item)
        QPoint calcPositionOfNextNode(const QModelIndex &) const;                                     // calculate position of next item (node item)
        QPoint calcPositionOfFirst(Data::ModelIndexInfoSet::Model::const_iterator) const;             // calculate position for first item
        QPoint calcPositionOfFirstImage() const;                                                      // calculate position for first item (image item)
        QPoint calcPositionOfFirstNode() const;                                                       // calculate position for first item (node item)

        int getItemWidth(const QModelIndex &) const;
        int getItemHeigth(const QModelIndex &) const;
        QSize getItemSize(const QModelIndex &) const;
        std::pair<int, int> selectRowFor(const QModelIndex &) const;
        int getFirstItemOffset() const;

        bool isRoot(const QModelIndex &) const;

        void updateItem(Data::ModelIndexInfoSet::Model::level_iterator) const;

        Data* m_data;
        const int m_width;
};

#endif // POSITIONSCALCULATOR_HPP
