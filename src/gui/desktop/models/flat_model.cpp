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


void FlatModel::setFilters(const std::vector<Database::IFilter::Ptr>& filters)
{
    {
        std::lock_guard<std::mutex> lock(m_filtersMutex);
        m_filters = filters;
    }

    updatePhotos();
}


const std::vector<Photo::Id>& FlatModel::photos() const
{
    return m_photos;
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
        m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
}


void FlatModel::updatePhotos()
{
    clearCaches();

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


std::vector<Database::IFilter::Ptr> FlatModel::filters() const
{
    std::lock_guard<std::mutex> lock(m_filtersMutex);

    return m_filters;
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
    const auto view_filters = filters();
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
    auto last_new_it = [&photos](){ return photos.end(); };
    auto last_old_it = [this](){ return m_photos.end(); };
    auto first_old_it = [this](){ return m_photos.begin(); };
    auto new_photos_it = photos.begin();
    auto old_photos_it = m_photos.begin();

    auto insertPhotos2 = [this](auto& output, auto& first, const auto& last) {
        insertPhotos(output, first, last);
        const auto items = std::distance(first, last);
        first = last;
        output += items;
    };

    if (m_photos.empty())
        insertPhotos2(old_photos_it, new_photos_it, photos.cend());
    else if (photos.empty())
        erasePhotos(first_old_it(), last_old_it());
    else
        while (new_photos_it != last_new_it() || old_photos_it != last_old_it())
        {
            if (new_photos_it != last_new_it() && old_photos_it == last_old_it())   // no more old, but still new ones?
            {
                insertPhotos(old_photos_it, new_photos_it, last_new_it());
                old_photos_it = last_old_it();
                new_photos_it = last_new_it();
                continue;
            }
            else if (new_photos_it == last_new_it() && old_photos_it != last_old_it())   // no more new, but still old ones?
            {
                old_photos_it = erasePhotos(old_photos_it, last_old_it());

                continue;
            }

            if (*new_photos_it == *old_photos_it)       // same photo on both sides - continue
            {
                ++new_photos_it;
                ++old_photos_it;

                continue;
            }

            // mismatch; check if new photo exists somewhere in old collection
            const auto position_in_old_collection = std::find(old_photos_it, last_old_it(), *new_photos_it);

            if (position_in_old_collection == m_photos.end())       // id of photo from new set doesn't exist in old one - insertion
            {
                // find last item from new set which doesn't occur in old block
                auto last_non_matching_new = std::next(new_photos_it);

                while (last_non_matching_new != last_new_it())
                {
                    auto it = std::find(old_photos_it, last_old_it(), *last_non_matching_new);

                    if (it == last_old_it())      // last_non_matching_new not found, keep looking
                    {
                        ++last_non_matching_new;
                        continue;
                    }
                    else                        // last_non_matching_new found in old sequence.
                    {
                        const auto run_length = std::distance(new_photos_it, std::prev(last_non_matching_new));

                        old_photos_it = insertPhotos(old_photos_it, new_photos_it, last_non_matching_new);  // insert block of new photos into old sequence

                        old_photos_it += run_length + 1;
                        new_photos_it = last_non_matching_new;

                        break;
                    }
                }

                if (last_non_matching_new == last_new_it())   // all items after new_photos_it where not found in old sequence.
                {
                    old_photos_it = erasePhotos(old_photos_it, last_old_it());    // everything in old sequence is not needed, remove it
                    old_photos_it = insertPhotos(old_photos_it, new_photos_it, last_new_it());

                    new_photos_it = last_new_it();
                    old_photos_it = last_old_it();
                }
            }
            else
                old_photos_it = erasePhotos(old_photos_it, position_in_old_collection);
        }

    assert(m_photos == photos);
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
