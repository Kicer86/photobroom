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

#ifndef MODELUNION_HPP
#define MODELUNION_HPP

#include <deque>

#include <QAbstractItemModel>

class ModelUnion: public QAbstractItemModel
{
    public:
        ModelUnion();
        virtual ~ModelUnion();

        void append(QAbstractItemModel *);

        QModelIndex index(int row, int column, const QModelIndex & parent) const override;
        QModelIndex parent(const QModelIndex & child) const override;

        int rowCount(const QModelIndex & parent) const override;
        int columnCount(const QModelIndex & parent) const override;

        QVariant data(const QModelIndex & index, int role) const override;

    private:
        std::deque<QAbstractItemModel *> m_subModels;
};

#endif // MODELUNION_HPP
