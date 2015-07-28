/*
 * Flat list model with lazy image load.
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

#ifndef LISTMODEL_HPP
#define LISTMODEL_HPP

#include <QAbstractItemModel>

class ListModel: public QAbstractItemModel
{
    public:
        ListModel();
        ListModel(const ListModel& other);
        ~ListModel();

        ListModel& operator=(const ListModel& other);
        bool operator==(const ListModel& other) const;

        virtual QVariant data(const QModelIndex& index, int role) const;
        virtual int columnCount(const QModelIndex& parent) const;
        virtual int rowCount(const QModelIndex& parent) const;
        virtual QModelIndex parent(const QModelIndex& child) const;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;

    private:
        class ListModelPrivate* const d;
};

#endif // LISTMODEL_HPP
