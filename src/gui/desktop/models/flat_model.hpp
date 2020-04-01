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
#include <QDate>

#include <database/filter.hpp>
#include "aphoto_info_model.hpp"
#include "photo_properties.hpp"

namespace Database
{
    struct IDatabase;
    struct IBackend;
}

class FlatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QDate timeRangeFrom READ timeRangeFrom NOTIFY timeRangeFromChanged)
    Q_PROPERTY(QDate timeRangeTo READ timeRangeTo NOTIFY timeRangeToChanged)
    Q_PROPERTY(QDate timeViewFrom READ timeViewFrom WRITE setTimeViewFrom)
    Q_PROPERTY(QDate timeViewTo READ timeViewTo WRITE setTimeViewTo)

    public:
        FlatModel(QObject* = nullptr);

        enum Roles
        {
            PhotoPropertiesRole = Qt::UserRole + 1,
            _lastRole = PhotoPropertiesRole,
        };

        void setDatabase(Database::IDatabase *);

        const QDate& timeRangeFrom() const;
        const QDate& timeRangeTo() const;
        const QDate& timeViewFrom() const;
        const QDate& timeViewTo() const;

        void setTimeViewFrom(const QDate &);
        void setTimeViewTo(const QDate &);

        QVariant data(const QModelIndex& index, int role) const override;
        int rowCount(const QModelIndex& parent) const override;
        QHash<int, QByteArray> roleNames() const override;

    signals:
        void timeRangeFromChanged() const;
        void timeRangeToChanged() const;

    private:
        std::vector<Photo::Id> m_photos;
        mutable std::map<Photo::Id, int> m_idToRow;
        mutable std::map<Photo::Id, PhotoProperties> m_properties;
        QPair<QDate, QDate> m_timeRange;
        QPair<QDate, QDate> m_timeView;
        Database::IDatabase* m_db;

        void reloadPhotos();
        void clearCaches();
        void setTimeRange(const QDate &, const QDate &);
        std::vector<Database::IFilter::Ptr> filters() const;
        std::vector<Database::IFilter::Ptr> viewFilters() const;

        PhotoProperties photoProperties(const Photo::Id &) const;
        void fetchPhotoProperties(const Photo::Id &) const;

        // methods working on backend
        void getTimeRangeForFilters(Database::IBackend *);
        void fetchMatchingPhotos(Database::IBackend *);
        void fetchPhotoProperties(Database::IBackend *, const Photo::Id &) const;

        // results from backend
        void fetchedPhotos(const std::vector<Photo::Id> &);
        void fetchedPhotoProperties(const Photo::Id &, const PhotoProperties &);
};

#endif // FLATMODEL_HPP
