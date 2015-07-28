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

#include "list_model.hpp"
#include "list_model_p.hpp"


ListModelPrivate::ListModelPrivate(ListModel* q) : q(q)
{
}


ListModelPrivate::~ListModelPrivate()
{
}



ListModel::ListModel()
    : d(new ListModelPrivate(this))
{

}

ListModel::ListModel(const ListModel& other)
    : d(new ListModelPrivate(this))
{

}


ListModel::~ListModel()
{
    delete d;

}


ListModel& ListModel::operator=(const ListModel& other)
{

}


bool ListModel::operator==(const ListModel& other) const
{

}


QVariant ListModel::data(const QModelIndex& index, int role) const
{

}


int ListModel::columnCount(const QModelIndex& parent) const
{

}


int ListModel::rowCount(const QModelIndex& parent) const
{

}

QModelIndex ListModel::parent(const QModelIndex& child) const
{

}


QModelIndex ListModel::index(int row, int column, const QModelIndex& parent) const
{

}
