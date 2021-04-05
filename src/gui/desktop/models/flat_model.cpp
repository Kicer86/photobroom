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

#include <tuple>

#include <core/function_wrappers.hpp>
#include <database/ibackend.hpp>
#include <database/idatabase.hpp>
#include <database/iphoto_operator.hpp>


/// @todo: get rid of const_cast and, if possible, remove mutables
using namespace std::placeholders;

FlatModel::FlatModel(QObject* p)
    : APhotoInfoModel(p)
    , m_db(nullptr)
{
}


void FlatModel::setDatabase(Database::IDatabase* db)
{
    if (m_db != nullptr)
    {
        auto& backend = m_db->backend();
        disconnect(&backend, &Database::IBackend::photosAdded,
                   this, &FlatModel::updatePhotos);

        disconnect(&backend, &Database::IBackend::photosRemoved,
                   this, &FlatModel::updatePhotos);

        disconnect(&backend, &Database::IBackend::photosModified,
                   this, &FlatModel::updatePhotos);
    }

    m_db = db;

    if (m_db != nullptr)
    {
        auto& backend = m_db->backend();
        connect(&backend, &Database::IBackend::photosAdded,
                this, &FlatModel::updatePhotos);

        connect(&backend, &Database::IBackend::photosRemoved,
                this, &FlatModel::updatePhotos);

        connect(&backend, &Database::IBackend::photosModified,
                this, &FlatModel::updatePhotos);
    }

    reloadPhotos();
}


void FlatModel::setFilter(const Database::Filter& filters)
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


const Database::Filter & FlatModel::filter() const
{
    return m_filters;
}


Database::IDatabase * FlatModel::database() const
{
    return m_db;
}


const Photo::Data& FlatModel::getPhotoData(const QModelIndex& index) const
{
    const int row = index.row();
    const Photo::Id id = m_photos[row];
    const Photo::Data& data = photoData(id);

    return data;
}


QVariant FlatModel::data(const QModelIndex& index, int role) const
{
    QVariant d;

    if (role == PhotoIdRole)
    {
        const int row = index.row();
        const Photo::Id id = m_photos[row];

        d = static_cast<int>(id);
    }
    else if (role == PhotoDataRole)
    {
        const Photo::Data& data = getPhotoData(index);
        d = QVariant::fromValue<Photo::Data>(data);
    }

    return d;
}


int FlatModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0 : static_cast<int>(m_photos.size());
}


int FlatModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: 1;  // only top parent has 1 column, all its items have 0
}


QModelIndex FlatModel::parent(const QModelIndex&) const
{
    return {};
}


QModelIndex FlatModel::index(int r, int c, const QModelIndex& p) const
{
    return p.isValid()? QModelIndex(): createIndex(r, c);
}


QUrl FlatModel::getPhotoPath(int row) const
{
    const Photo::Id id = m_photos[row];
    const Photo::Data& data = photoData(id);
    const QUrl url = QUrl::fromLocalFile(data.path);

    return url;
}


void FlatModel::reloadPhotos()
{
    resetModel();

    if (m_db != nullptr)
        m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
}


void FlatModel::updatePhotos()
{
    if (m_db != nullptr)
        m_db->exec(std::bind(&FlatModel::fetchMatchingPhotos, this, _1));
}


void FlatModel::removeAllPhotos()
{
    m_properties.clear();
    m_idToRow.clear();
    m_photos.clear();
}


void FlatModel::resetModel()
{
    beginResetModel();
    removeAllPhotos();
    endResetModel();
}


const Database::Filter& FlatModel::filters() const
{
    std::lock_guard<std::mutex> lock(m_filtersMutex);

    return m_filters;
}


const Photo::Data& FlatModel::photoData(const Photo::Id& id) const
{
    auto it = m_properties.find(id);

    if (it == m_properties.end())
    {
        fetchPhotoData(id);
        std::tie(it, std::ignore) = m_properties.emplace(id, Photo::Data());   // insert empty properties so we won't call fetchPhotoProperties() for this 'id' again
    }

    return it->second;
}


void FlatModel::fetchPhotoData(const Photo::Id& id) const
{
    auto b = std::bind(qOverload<Database::IBackend &, const Photo::Id &>(&FlatModel::fetchPhotoProperties), this, _1, id);

    m_db->exec(b);
}


void FlatModel::fetchMatchingPhotos(Database::IBackend& backend)
{
    const Database::Actions::GroupAction sort_action({
        Database::Actions::SortByTimestamp(),
        Database::Actions::SortByID()
    });

    const auto view_filters = filters();
    const auto photos = backend.photoOperator().onPhotos(view_filters, sort_action);

    invokeMethod(this, &FlatModel::fetchedPhotos, photos);
}


void FlatModel::fetchPhotoProperties(Database::IBackend& backend, const Photo::Id& id) const
{
    auto photo = backend.getPhoto(id);

    invokeMethod(const_cast<FlatModel*>(this), &FlatModel::fetchedPhotoProperties, id, photo);
}


void FlatModel::fetchedPhotos(const std::vector<Photo::Id>& photos)
{
    auto last_new_it = [&photos](){ return photos.end(); };
    auto last_old_it = [this](){ return m_photos.end(); };
    auto new_photos_it = photos.begin();
    auto old_photos_it = m_photos.begin();

    auto insertPhotosAndUpdate = [this](auto& output, auto& first, const auto& last) {
        output = insertPhotos(output, first, last);
        const auto items = std::distance(first, last);
        first = last;
        output += items;
    };

    if (m_photos.empty())
        insertPhotosAndUpdate(old_photos_it, new_photos_it, photos.cend());
    else if (photos.empty())
        erasePhotos(old_photos_it, last_old_it());
    else
        while (new_photos_it != last_new_it() || old_photos_it != last_old_it())
        {
            if (new_photos_it != last_new_it() && old_photos_it == last_old_it())   // no more old, but still new ones?
            {
                insertPhotosAndUpdate(old_photos_it, new_photos_it, last_new_it());
                continue;
            }
            else if (new_photos_it == last_new_it() && old_photos_it != last_old_it())   // no more new, but still old ones?
            {
                erasePhotos(old_photos_it, last_old_it());
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
                        insertPhotosAndUpdate(old_photos_it, new_photos_it, last_non_matching_new);  // insert block of new photos into old sequence
                        break;
                    }
                }

                if (last_non_matching_new == last_new_it())   // all items after new_photos_it where not found in old sequence.
                {
                    old_photos_it = erasePhotos(old_photos_it, last_old_it());    // everything in old sequence is not needed, remove it
                    insertPhotosAndUpdate(old_photos_it, new_photos_it, last_new_it());
                }
            }
            else
                old_photos_it = erasePhotos(old_photos_it, position_in_old_collection);
        }

    assert(m_photos == photos);

    m_idToRow.clear();
    for(std::size_t i = 0; i < m_photos.size(); i++)
        m_idToRow.emplace(m_photos[i], i);
}


void FlatModel::fetchedPhotoProperties(const Photo::Id& id, const Photo::Data& properties)
{
    auto it = m_idToRow.find(id);

    const int row = it == m_idToRow.end()? -1 : it->second;
    assert(row != -1);

    m_properties[id] = properties;

    if (row != -1)
    {
        const QModelIndex idx = createIndex(row, 0);
        emit dataChanged(idx, idx, {PhotoDataRole});
    }
}
