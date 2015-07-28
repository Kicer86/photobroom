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

#include "image_list_model.hpp"
#include "image_list_model_p.hpp"

#include <QFileInfo>


ImageListModelPrivate::ImageListModelPrivate(ImageListModel* q): q(q), m_data(), m_taskExecutor(nullptr)
{
}


ImageListModelPrivate::~ImageListModelPrivate()
{
}


///////////////////////////////////////////////////////////////////////////////


ImageListModel::ImageListModel(QObject* p):
    QAbstractItemModel(p),
    d(new ImageListModelPrivate(this))
{

}


ImageListModel::ImageListModel(const ImageListModel& other)
    : d(new ImageListModelPrivate(this))
{

}


ImageListModel::~ImageListModel()
{
    delete d;
}


ImageListModel& ImageListModel::operator=(const ImageListModel& other)
{

}


bool ImageListModel::operator==(const ImageListModel& other) const
{

}


void ImageListModel::insert(const QString& path)
{
    const int s = d->m_data.size();

    beginInsertRows(QModelIndex(), s, s);

    d->m_data.push_back(path);

    endInsertRows();
}


void ImageListModel::clear()
{
    beginResetModel();

    d->m_data.clear();

    endResetModel();
}


void ImageListModel::set(ITaskExecutor* taskExecutor)
{
    d->m_taskExecutor = taskExecutor;
}


QVariant ImageListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid())
    {
        Info& info = d->m_data[index.row()];

        switch(role)
        {
            case Qt::DisplayRole:
            {
                if (info.filename.isEmpty())
                {
                    QFileInfo file_info(info.path);
                    info.filename = file_info.fileName();
                }

                result = info.filename;
                break;
            }

            case Qt::DecorationRole:
            {
                if (info.pixmap.isNull())
                {
                    QPixmap pixmap(info.path);
                    info.pixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                result = info.pixmap;
            }

            default:
                break;
        }
    }

    return result;
}


int ImageListModel::columnCount(const QModelIndex& parent) const
{
    const int result = parent.isValid()? 0 : 1;

    return result;
}


int ImageListModel::rowCount(const QModelIndex& parent) const
{
    const int result = parent.isValid()? 0: d->m_data.size();

    return result;
}


QModelIndex ImageListModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}


QModelIndex ImageListModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, nullptr);
}
