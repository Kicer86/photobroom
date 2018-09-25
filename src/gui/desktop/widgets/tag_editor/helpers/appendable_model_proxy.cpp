/*
 * A model proxy which adds extra row for data appending.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "appendable_model_proxy.hpp"


AppendableModelProxy::AppendableModelProxy()
{
}


AppendableModelProxy::~AppendableModelProxy()
{
}


QVariant AppendableModelProxy::data(const QModelIndex& index, int role) const
{

}

int AppendableModelProxy::columnCount(const QModelIndex& parent) const
{

}

int AppendableModelProxy::rowCount(const QModelIndex& parent) const
{

}

QModelIndex AppendableModelProxy::parent(const QModelIndex& child) const
{

}

QModelIndex AppendableModelProxy::index(int row, int column, const QModelIndex& parent) const
{

}
