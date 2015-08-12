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

#ifndef IMAGELISTMODEL_HPP
#define IMAGELISTMODEL_HPP

#include <QAbstractItemModel>

class ITaskExecutor;

class ImageListModel: public QAbstractItemModel
{
    public:
        ImageListModel(QObject *);
        ImageListModel(const ImageListModel& other);
        ~ImageListModel();

        ImageListModel& operator=(const ImageListModel& other);
        bool operator==(const ImageListModel& other) const;

        void insert(const QString &);
        void clear();
        const QString get(const QModelIndex &) const;

        void set(ITaskExecutor *);

        // QAbstractItemModel:
        QVariant data(const QModelIndex& index, int role) const override;
        int columnCount(const QModelIndex& parent) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& child) const override;
        QModelIndex index(int row, int column, const QModelIndex& parent) const override;

    private:
        class ImageListModelPrivate* const d;

        void imageScaled(const QString &, const QImage &);
};

#endif // IMAGELISTMODEL_HPP
