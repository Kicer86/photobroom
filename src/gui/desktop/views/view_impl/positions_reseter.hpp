/*
 * Class for reseting items sizes and positions.
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

#ifndef POSITIONSRESETER_H
#define POSITIONSRESETER_H

#include "data.hpp"

class QAbstractItemModel;
class QModelIndex;
class QItemSelection;

class PositionsReseter
{
    public:
        PositionsReseter(QAbstractItemModel *, Data *);
        PositionsReseter(const PositionsReseter &) = delete;
        ~PositionsReseter();
        PositionsReseter& operator=(const PositionsReseter &) = delete;

        void itemsAdded(const QModelIndex& parent, int from_pos, int to_pos) const;
        void invalidateAll() const;
        void itemChanged(const QModelIndex &);
        void itemsChanged(const QItemSelection &);
        void childRemoved(const QModelIndex& parent, int pos);

    private:
        Data* m_data;
        QAbstractItemModel* m_model;

        void invalidateItemOverallSize(const QModelIndex &) const;
        void invalidateSiblingsRect(const QModelIndex &) const;
        void invalidateSiblingsPosition(const QModelIndex &) const;
        void invalidateChildrenRect(const QModelIndex &, int from = 0) const;

        void resetRect(const QModelIndex &) const;
        void resetPosition(const QModelIndex &) const;
        void resetSize(const QModelIndex &) const;
        void resetOverallSize(const QModelIndex &) const;
};

#endif // POSITIONSRESETER_H
