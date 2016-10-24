/*
 * Model which combines two or more models.
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#include "model_union.hpp"

ModelUnion::ModelUnion():
    QAbstractItemModel(),
    m_subModels()
{

}


ModelUnion::~ModelUnion()
{

}


void ModelUnion::append(QAbstractItemModel* model)
{
    m_subModels.push_back(model);
}


QModelIndex ModelUnion::index(int row, int column, const QModelIndex& parent) const
{

}


QModelIndex ModelUnion::parent(const QModelIndex& child) const
{

}


int ModelUnion::rowCount(const QModelIndex& parent) const
{
    const int count = std::accumulate(m_subModels.begin(), m_subModels.end(), 0, [this, parent](int current_row_count, const QAbstractItemModel* model)
    {
        return current_row_count + model->rowCount(parent);
    });

    return count;
}


int ModelUnion::columnCount(const QModelIndex& parent) const
{
    int count = 0;

    for(const QAbstractItemModel* model: m_subModels)
    {
        const int c = model->columnCount();
        if (count < c)
            count = c;
    };

    return count;
}


QVariant ModelUnion::data(const QModelIndex& index, int role) const
{

}
