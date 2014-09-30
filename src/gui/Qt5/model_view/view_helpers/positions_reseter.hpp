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

class QModelIndex;
class Data;

class PositionsReseter
{
public:
    PositionsReseter(Data *);
    PositionsReseter(const PositionsReseter &) = delete;
    ~PositionsReseter();
    PositionsReseter& operator=(const PositionsReseter &) = delete;

    void itemsAdded(const QModelIndex &, int last) const;
    void invalidateAll() const;

private:
    Data* m_data;

    void invalidateItemOverallRect(const QModelIndex &) const;
    void invalidateSiblingsRect(const QModelIndex &) const;

    void resetRect(const QModelIndex &) const;
    void resetOverallRect(const QModelIndex &) const;
};

#endif // POSITIONSRESETER_H
