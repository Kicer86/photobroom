/*
 * Database based data model
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

#ifndef DBDATAMODEL_H
#define DBDATAMODEL_H

#include <QModelIndex>

class DBDataModel :  QAbstractListModel
{
    public:
        DBDataModel();
        DBDataModel(const DBDataModel& other) = delete;
        ~DBDataModel();

        DBDataModel& operator=(const DBDataModel& other) = delete;
        bool operator==(const DBDataModel& other) = delete;

        virtual QVariant data(const QModelIndex& index, int role);
        virtual int rowCount(const QModelIndex& parent);
        virtual void fetchMore(const QModelIndex& parent);
        virtual bool canFetchMore(const QModelIndex& parent);
};

#endif // DBDATAMODEL_H
