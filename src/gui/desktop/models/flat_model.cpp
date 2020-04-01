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


/// @todo: get rid of const_cast and, if possible, remove mutables
using namespace std::placeholders;

FlatModel::FlatModel(QObject* p)
    : QAbstractListModel(p)
    , m_db(nullptr)
{
}


void FlatModel::setDatabase(Database::IDatabase* db)
{
    m_db = db;
    reloadPhotos();
}


const QDate& FlatModel::timeRangeFrom() const
{
    return m_timeRange.first;
}


const QDate& FlatModel::timeRangeTo() const
{
    return m_timeRange.second;
}


const QDate& FlatModel::timeViewFrom() const
{
    return m_timeView.first;
}


const QDate& FlatModel::timeViewTo() const
{
    return m_timeView.second;
}


void FlatModel::setTimeViewFrom(const QDate& viewFrom)
{
    m_timeView.first = viewFrom;
    reloadView();
}


void FlatModel::setTimeViewTo(const QDate& viewTo)
{
    m_timeView.second = viewTo;
    reloadView();
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


void FlatModel::reloadPhotos()
{
    resetModel();

    if (m_db != nullptr)
        m_db->exec(std::bind(&FlatModel::getTimeRangeForFilters, this, _1));
}


void FlatModel::reloadView()
{
    resetModel();

    if (m_db != nullptr)
        m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
}


void FlatModel::clearCaches()
{
    m_idToRow.clear();
    m_properties.clear();
}


void FlatModel::removeAllPhotos()
{
    m_photos.clear();
    clearCaches();
}


void FlatModel::resetModel()
{
    beginResetModel();
    removeAllPhotos();
    endResetModel();
}


void FlatModel::setTimeRange(const QDate& from, const QDate& to)
{
    const QPair newTimeRange(from, to);

    if (newTimeRange != m_timeRange)
    {
        m_timeRange = QPair(from, to);
        m_timeView = m_timeRange;

        emit timeRangeFromChanged();
        emit timeRangeToChanged();

        m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
    }
}


std::vector<Database::IFilter::Ptr> FlatModel::filters() const
{
    /// @todo: make me thread safe
    return {};
}


std::vector<Database::IFilter::Ptr> FlatModel::viewFilters() const
{
    /// @todo: make me thread safe
    auto view_filters = filters();

    view_filters.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, m_timeView.first, Database::FilterPhotosWithTag::ValueMode::GreaterOrEqual) );
    view_filters.push_back( std::make_shared<Database::FilterPhotosWithTag>(TagTypes::Date, m_timeView.second, Database::FilterPhotosWithTag::ValueMode::LessOrEqual) );

    return view_filters;
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


void FlatModel::getTimeRangeForFilters(Database::IBackend* backend)
{
    const auto range_filters = filters();
    const auto dates = backend->listTagValues(TagTypes::Date, range_filters);

    if (dates.empty() == false)
    {
        const auto dates_range = std::minmax_element(dates.begin(), dates.end());

        invokeMethod(this, &FlatModel::setTimeRange, dates_range.first->getDate(), dates_range.second->getDate());
    }
    else
        invokeMethod(this, &FlatModel::setTimeRange, QDate(), QDate());
}


void FlatModel::fetchMatchingPhotos(Database::IBackend* backend)
{
    const auto view_filters = viewFilters();
    const auto photos = backend->getPhotos(view_filters);

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
