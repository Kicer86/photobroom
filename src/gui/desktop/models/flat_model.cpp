/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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

#include "flat_model.hpp"

#include <core/function_wrappers.hpp>
#include <database/ibackend.hpp>
#include <database/idatabase.hpp>


using namespace std::placeholders;

FlatModel::FlatModel(QObject* p)
    : QAbstractListModel(p)
    , m_db(nullptr)
{
}


void FlatModel::setDatabase(Database::IDatabase* db)
{
    m_db = db;

    m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
}


QVariant FlatModel::data(const QModelIndex& index, int role) const
{
    QVariant d;

    if (role == PhotoIdRole)
        d = static_cast<int>(m_photos[index.row()]);

    return d;
}


int FlatModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0 : static_cast<int>(m_photos.size());
}


QHash<int, QByteArray> FlatModel::roleNames() const
{
    QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
    result.insert(PhotoIdRole, "photoId");

    return result;
}


void FlatModel::fetchedPhotos(const std::vector<Photo::Id>& photos)
{
    beginResetModel();
    m_photos = photos;
    endResetModel();
}


void FlatModel::fetchMatchingPhotos(Database::IBackend* backend)
{
    auto photos = backend->getPhotos({});

    invokeMethod(this, &FlatModel::fetchedPhotos, photos);
}
