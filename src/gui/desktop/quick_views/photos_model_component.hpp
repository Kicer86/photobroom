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

#ifndef PHOTOSMODELCOMPONENT_HPP
#define PHOTOSMODELCOMPONENT_HPP


#include <QDate>
#include <QObject>

#include <database/idatabase.hpp>
#include <database/filter.hpp>

class QAbstractItemModel;
class FlatModel;


class PhotosModelComponent: public QObject
{
        Q_OBJECT

        Q_PROPERTY(QAbstractItemModel* model READ model NOTIFY modelChanged)
        Q_PROPERTY(QDate timeRangeFrom READ timeRangeFrom NOTIFY timeRangeFromChanged)
        Q_PROPERTY(QDate timeRangeTo READ timeRangeTo NOTIFY timeRangeToChanged)
        Q_PROPERTY(QDate timeViewFrom READ timeViewFrom WRITE setTimeViewFrom)
        Q_PROPERTY(QDate timeViewTo READ timeViewTo WRITE setTimeViewTo)

    public:
        PhotosModelComponent(QObject * = nullptr);

        void setDatabase(Database::IDatabase *);

        QAbstractItemModel* model() const;
        const QDate& timeRangeFrom() const;
        const QDate& timeRangeTo() const;
        const QDate& timeViewFrom() const;
        const QDate& timeViewTo() const;

        void setTimeViewFrom(const QDate &);
        void setTimeViewTo(const QDate &);

    signals:
        void modelChanged() const;
        void timeRangeFromChanged() const;
        void timeRangeToChanged() const;

    private:
        QPair<QDate, QDate> m_timeRange;
        QPair<QDate, QDate> m_timeView;
        FlatModel* m_model;
        Database::IDatabase* m_db;

        void setTimeRange(const QDate &, const QDate &);
        void updateTimeRange();
        std::vector<Database::IFilter::Ptr> filters() const;

        void getTimeRangeForFilters(Database::IBackend *);
};

#endif // PHOTOSMODELCOMPONENT_HPP
