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

#ifndef FLATMODEL_HPP
#define FLATMODEL_HPP

#include <map>
#include <mutex>
#include <QDate>

#include <database/filter.hpp>
#include "aphoto_info_model.hpp"
#include "photo_properties.hpp"

namespace Database
{
    struct IDatabase;
    struct IBackend;
}

class FlatModel: public QAbstractListModel
{
    Q_OBJECT

    public:
        FlatModel(QObject* = nullptr);

        enum Roles
        {
            PhotoPropertiesRole = Qt::UserRole + 1,
            _lastRole = PhotoPropertiesRole,
        };

        void setDatabase(Database::IDatabase *);
        void setFilters(const std::vector<Database::IFilter::Ptr> &);
        const std::vector<Photo::Id>& photos() const;

        QVariant data(const QModelIndex& index, int role) const override;
        int rowCount(const QModelIndex& parent) const override;
        QHash<int, QByteArray> roleNames() const override;

    private:
        std::vector<Database::IFilter::Ptr> m_filters;
        std::vector<Photo::Id> m_photos;
        mutable std::mutex m_filtersMutex;
        mutable std::map<Photo::Id, int> m_idToRow;
        mutable std::map<Photo::Id, PhotoProperties> m_properties;
        Database::IDatabase* m_db;

        void reloadPhotos();
        void updatePhotos();
        void clearCaches();
        void removeAllPhotos();
        void resetModel();
        std::vector<Database::IFilter::Ptr> filters() const;

        PhotoProperties photoProperties(const Photo::Id &) const;
        void fetchPhotoProperties(const Photo::Id &) const;

        // methods working on backend
        void fetchMatchingPhotos(Database::IBackend *);
        void fetchPhotoProperties(Database::IBackend *, const Photo::Id &) const;

        // results from backend
        void fetchedPhotos(const std::vector<Photo::Id> &);
        void fetchedPhotoProperties(const Photo::Id &, const PhotoProperties &);

        // altering model
        template<typename T>
        decltype(m_photos)::iterator insertPhotos(decltype(m_photos)::iterator position_it, T first, T last)
        {
            if (first == last)
                return position_it;

            const auto items = static_cast<int>(std::distance(first, last));
            const auto position = static_cast<int>(std::distance(m_photos.begin(), position_it));

            beginInsertRows({}, position, position + items - 1);
            auto r = m_photos.insert(position_it, first, last);
            endInsertRows();

            return r;
        }

        decltype(m_photos)::iterator erasePhotos(decltype(m_photos)::iterator first, decltype(m_photos)::iterator last)
        {
            if (first == last)
                return first;

            const auto items = static_cast<int>(std::distance(first, last));
            const auto position = static_cast<int>(std::distance(m_photos.begin(), first));

            beginRemoveRows({}, position, position + items - 1);
            auto r = m_photos.erase(first, last);
            endRemoveRows();

            return r;
        }
};

#endif // FLATMODEL_HPP
