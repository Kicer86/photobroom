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
        PositionsCalculator(QAbstractItemModel *, Data *, int width);
        PositionsCalculator(const PositionsCalculator& other) = delete;
        ~PositionsCalculator();
        PositionsCalculator& operator=(const PositionsCalculator& other) = delete;

        void updateItems() const;

    private:
        QRect calcItemRect(Data::ModelIndexInfoSet::flat_iterator) const;
        QSize calcItemSize(Data::ModelIndexInfoSet::flat_iterator) const;
        QPoint calcItemPosition(Data::ModelIndexInfoSet::flat_iterator) const;
        QPoint positionOfNext(Data::ModelIndexInfoSet::flat_iterator) const;          // calculate position of next item
        QPoint positionOfNextImage(Data::ModelIndexInfoSet::flat_iterator) const;     // calculate position of next item (image item)
        QPoint positionOfNextNode(Data::ModelIndexInfoSet::flat_iterator) const;      // calculate position of next item (node item)
        QPoint positionOfFirstChild(Data::ModelIndexInfoSet::flat_iterator) const;    // calculate position for first item

        int getItemWidth(Data::ModelIndexInfoSet::flat_iterator) const;
        int getItemHeigth(Data::ModelIndexInfoSet::flat_iterator) const;
        QSize getItemSize(Data::ModelIndexInfoSet::flat_iterator) const;
        std::pair<int, int> selectRowFor(Data::ModelIndexInfoSet::flat_iterator) const;

        bool isRoot(Data::ModelIndexInfoSet::flat_iterator) const;

        void updateItems(Data::ModelIndexInfoSet::flat_iterator) const;
        void updateItem(Data::ModelIndexInfoSet::flat_iterator) const;

        QAbstractItemModel* m_model;
        Data* m_data;
        const int m_width;
};

#endif // POSITIONSCALCULATOR_HPP
