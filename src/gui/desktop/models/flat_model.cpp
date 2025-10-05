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
#include <database/iphoto_operator.hpp>

#include "gui/desktop/utils/photo_delta_fetcher_setup.hpp"


namespace
{
    template<std::forward_iterator T>
    T findLastConsecutive(T first, T last)
    {
        T lastConsecutive = first;
        auto value = *first++;

        while(first != last)
        {
            if (*first - value != 1)
                break;

            lastConsecutive = first;
            value = *first++;
        }

        return lastConsecutive;
    }

    template<std::forward_iterator T, typename O>
    void findConsecutiveRanges(T first, T last, O output)
    {
        for(; first != last; ++first)
        {
            auto lit = findLastConsecutive(first, last);

            *output++ = std::make_pair(*first, *lit);
            first = lit;
        }
    }
}


/// @todo: get rid of const_cast and, if possible, remove mutables
using namespace std::placeholders;

FlatModel::FlatModel(QObject* p)
    : APhotoDataModel(p)
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
                   this, &FlatModel::removePhotos);

        disconnect(&backend, &Database::IBackend::photosModified,
                   this, &FlatModel::invalidatePhotos);
    }

    m_db = db;

    if (m_db != nullptr)
    {
        auto& backend = m_db->backend();
        connect(&backend, &Database::IBackend::photosAdded,
                this, &FlatModel::updatePhotos);

        connect(&backend, &Database::IBackend::photosRemoved,
                this, &FlatModel::removePhotos);

        connect(&backend, &Database::IBackend::photosModified,
                this, &FlatModel::invalidatePhotos);
    }

    Gui::Utils::resetPhotoDeltaFetcher(m_translator, m_db, this, &FlatModel::gotPhotoData);

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


const Database::Filter& FlatModel::filter() const
{
    return m_filters;
}


Database::IDatabase* FlatModel::database() const
{
    return m_db;
}


const APhotoDataModel::ExplicitDelta& FlatModel::getPhotoData(const QModelIndex& index) const
{
    const int row = index.row();
    const Photo::Id id = m_photos[row];
    const auto& data = photoData(id);

    return data;
}


QVariant FlatModel::data(const QModelIndex& index, int role) const
{
    QVariant d;

    if (role == PhotoIdRole)
    {
        const int row = index.row();
        const Photo::Id id = m_photos[row];

        d = id.variant();
    }
    else if (role == PhotoDataRole)
    {
        const auto& data = getPhotoData(index);
        d = QVariant::fromValue<Photo::DataDelta>(static_cast<Photo::DataDelta>(data));
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
    const auto& data = photoData(id);
    const QUrl url = QUrl::fromLocalFile(data.get<Photo::Field::Path>());

    return url;
}


Photo::Id FlatModel::getId(int row) const
{
    assert(row >= 0 && row < static_cast<int>(m_photos.size()));

    return m_photos[row];
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


void FlatModel::removePhotos(const std::vector<Photo::Id>& idsToBeRemoved)
{
    std::vector<int> rowsToBeRemoved;
    rowsOfIds(idsToBeRemoved.begin(), idsToBeRemoved.end(), std::back_inserter(rowsToBeRemoved));

    std::ranges::sort(rowsToBeRemoved);

    std::vector<std::pair<int, int>> rangesToBeRemoved;
    findConsecutiveRanges(rowsToBeRemoved.begin(), rowsToBeRemoved.end(), std::back_inserter(rangesToBeRemoved));

    // remove from back
    std::ranges::reverse(rangesToBeRemoved);

    for (const auto& range: rangesToBeRemoved)
    {
        erasePhotos(m_photos.begin() + range.first,
                    m_photos.begin() + range.second + 1);
    }
}


void FlatModel::invalidatePhotos(const std::set<Photo::Id>& ids)
{
    for(const Photo::Id& id: ids)
    {
        const auto it = m_properties.find(id);

        if (it != m_properties.end())
            m_properties.erase(it);
    }

    std::vector<int> rowsToBeInvalidated;
    rowsOfIds(ids.begin(), ids.end(), std::back_inserter(rowsToBeInvalidated));

    std::vector<std::pair<int, int>> rangesToBeInvalidated;
    findConsecutiveRanges(rowsToBeInvalidated.begin(), rowsToBeInvalidated.end(), std::back_inserter(rangesToBeInvalidated));

    for(const auto& range: rangesToBeInvalidated)
        emit dataChanged(indexForRow(range.first), indexForRow(range.second));

    if (rowsToBeInvalidated.size() != ids.size())
    {
        // Some of the photos which changed were not matching model's filters.
        // Maybe they do now? Update model.
        // TODO: currently all photos are being refetched, try to be smarter and only fetch invalidated photos.
        updatePhotos();
    }
}


const Database::Filter& FlatModel::filters() const
{
    std::lock_guard<std::mutex> lock(m_filtersMutex);

    return m_filters;
}


const APhotoDataModel::ExplicitDelta& FlatModel::photoData(const Photo::Id& id) const
{
    assert(m_idToRow.find(id) != m_idToRow.end());

    auto it = m_properties.find(id);

    if (it == m_properties.end())
    {
        fetchPhotoData(id);

        // insert empty properties so we won't call fetchPhotoProperties() for this 'id' again
        std::tie(it, std::ignore) = m_properties.emplace(id, ExplicitDelta(id));
    }

    return it->second;
}


void FlatModel::fetchPhotoData(const Photo::Id& id) const
{
    if (m_translator)
        m_translator->fetchIds({id}, {Photo::Field::Path, Photo::Field::Flags, Photo::Field::GroupInfo});
}


void FlatModel::fetchMatchingPhotos(Database::IBackend& backend)
{
    const Database::Actions::GroupAction sort_action({
        Database::Actions::Sort(Database::Actions::Sort::By::Timestamp),
        Database::Actions::Sort(Database::Actions::Sort::By::ID)
    });

    const auto view_filters = filters();
    const auto photos = backend.photoOperator().onPhotos(view_filters, sort_action);

    invokeMethod(this, &FlatModel::fetchedPhotos, photos);
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

    assert(m_idToRow.size() == m_photos.size());
}


void FlatModel::gotPhotoData(const std::vector<Photo::DataDelta>& data)
{
    const auto deltas = Photo::EDV<ExplicitDelta>(data);

    for (const auto& delta: deltas)
        fetchedPhotoProperties(delta.getId(), delta);
}


void FlatModel::fetchedPhotoProperties(const Photo::Id& id, const ExplicitDelta& properties)
{
    auto it = m_idToRow.find(id);

    // photo may have been removed from model in the meantime (between fetchPhotoProperties and fetchedPhotoProperties execution)
    if (it != m_idToRow.end())
    {
        const int row = it->second;
        m_properties[id] = properties;

        const QModelIndex idx = createIndex(row, 0);
        emit dataChanged(idx, idx, {PhotoDataRole});
    }
}


QModelIndex FlatModel::indexForRow(int r) const
{
    return createIndex(r, 0);
}
