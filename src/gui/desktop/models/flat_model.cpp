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
    beginResetModel();
    m_db = db;
    m_idToRow.clear();
    m_photos.clear();
    m_properties.clear();
    endResetModel();

    if (m_db != nullptr)
        m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
}


QPair<QDate, QDate> FlatModel::timeRange() const
{
    return m_timeRange;
}


QVariant FlatModel::data(const QModelIndex& index, int role) const
{
    QVariant d;

    if (role == PhotoPropertiesRole)
    {
        const int row = index.row();
        const Photo::Id id = m_photos[row];
        m_idToRow[id] = row;
        const PhotoProperties properties = photoProperties(id);
        d = QVariant::fromValue<PhotoProperties>(properties);
    }

    return d;
}


int FlatModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0 : static_cast<int>(m_photos.size());
}


QHash<int, QByteArray> FlatModel::roleNames() const
{
    QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
    result.insert(PhotoPropertiesRole, "photoProperties");

    return result;
}


void FlatModel::setTimeRange(const QDate& from, const QDate& to)
{
    m_timeRange = QPair(from, to);

    emit timeRangeChanged();
}


std::vector<Database::IFilter::Ptr> FlatModel::filters() const
{
    return {};
}


PhotoProperties FlatModel::photoProperties(const Photo::Id& id) const
{
    PhotoProperties properties;

    auto it = m_properties.find(id);

    if (it == m_properties.end())
    {
        fetchPhotoProperties(id);
        m_properties.emplace(id, properties);   // insert empty properties so we won't call fetchPhotoProperties() for this 'id' again
    }
    else
        properties = it->second;

    return properties;
}


void FlatModel::fetchPhotoProperties(const Photo::Id& id) const
{
    auto b = std::bind(qOverload<Database::IBackend *, const Photo::Id &>(&FlatModel::fetchPhotoProperties), this, _1, id);

    m_db->exec(b);
}


void FlatModel::fetchMatchingPhotos(Database::IBackend* backend)
{
    auto photos = backend->getPhotos({});

    invokeMethod(this, &FlatModel::fetchedPhotos, photos);
}


void FlatModel::fetchPhotoProperties(Database::IBackend* backend, const Photo::Id& id) const
{
    auto photo = backend->getPhoto(id);
    const PhotoProperties properties(photo.path, photo.geometry);

    invokeMethod(const_cast<FlatModel*>(this), &FlatModel::fetchedPhotoProperties, id, properties);
}


void FlatModel::fetchedPhotos(const std::vector<Photo::Id>& photos)
{
    const int size = static_cast<int>(photos.size());

    beginInsertRows({}, 0, size - 1);
    m_photos = photos;
    endInsertRows();
}


void FlatModel::fetchedPhotoProperties(const Photo::Id& id, const PhotoProperties& properties)
{
    auto it = m_idToRow.find(id);

    const int row = it == m_idToRow.end()? -1 : it->second;
    assert(row != -1);

    m_properties[id] = properties;

    if (row != -1)
    {
        const QModelIndex idx = createIndex(row, 0);
        emit dataChanged(idx, idx, {PhotoPropertiesRole});
    }
}
