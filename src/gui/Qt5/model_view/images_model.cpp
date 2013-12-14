/*
    Data Model for Images.
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "images_model.hpp"

#include <memory>

#include <QPixmap>


ImagesModel::ImagesModel() : QAbstractListModel(), m_photos() {}


ImagesModel::~ImagesModel()
{
}


void ImagesModel::add(const APhotoInfo& photoInfo)
{
    QModelIndex parentIndex;
    const int items = m_photos.size();

    beginInsertRows(parentIndex, items, items);

    APhotoInfo::Ptr photo = std::make_shared<APhotoInfo>(photoInfo);
    m_photos.push_back(photo);

    endInsertRows();
}


APhotoInfo::Ptr ImagesModel::get(const QModelIndex& idx)
{
    const int row = idx.row();
    APhotoInfo::Ptr result = m_photos[row];

    return result;
}


const std::vector<APhotoInfo::Ptr>& ImagesModel::getAll() const
{
    return m_photos;
}


int ImagesModel::rowCount(const QModelIndex&) const
{
    return m_photos.size();
}


QVariant ImagesModel::data(const QModelIndex& _index, int role) const
{
    const int row = _index.row();
    const APhotoInfo::Ptr info = m_photos[row];

    QVariant result;

    switch(role)
    {
        case Qt::DisplayRole:
            result = info->getPath();
            break;

        case Qt::DecorationRole:
            result = info->getPixmap();
            break;

        default:
            break;
    }

    return result;
}

