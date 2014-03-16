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

#include "db_data_model.hpp"


struct DBDataModel::Impl
{
    Impl() {}
    ~Impl() {}
};


DBDataModel::DBDataModel(): QAbstractItemModel()
{

}


DBDataModel::~DBDataModel()
{

}


bool DBDataModel::canFetchMore(const QModelIndex& parent) const
{

}


void DBDataModel::fetchMore(const QModelIndex& parent)
{

}


int DBDataModel::columnCount(const QModelIndex& parent) const
{

}


QVariant DBDataModel::data(const QModelIndex& index, int role) const
{

}


QModelIndex DBDataModel::index(int row, int column, const QModelIndex& parent) const
{

}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{

}


int DBDataModel::rowCount(const QModelIndex& parent) const
{

}
