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
#include <QTimer>
#include <QStandardItemModel>

#include <database/idatabase.hpp>
#include <database/filter.hpp>
#include "models/flat_model.hpp"


struct ICompleterFactory;

/* \brief Main bridge between QML Photo View and C++ world */
class PhotosModelControllerComponent: public QObject
{
        Q_OBJECT

        // getters
        Q_PROPERTY(QAbstractItemModel* photos READ model CONSTANT)
        Q_PROPERTY(unsigned int datesCount READ datesCount NOTIFY datesCountChanged)
        Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)

        // setters - filters narrowing view
        Q_PROPERTY(unsigned int timeViewFrom READ timeViewFrom WRITE setTimeViewFrom)
        Q_PROPERTY(unsigned int timeViewTo READ timeViewTo WRITE setTimeViewTo)
        Q_PROPERTY(QString searchExpression READ searchExpression WRITE setSearchExpression)
        Q_PROPERTY(bool newPhotosOnly READ newPhotosOnly WRITE setNewPhotosOnly)
        Q_PROPERTY(int category READ category WRITE setCategory)
        Q_PROPERTY(float ratingFrom READ ratingFrom WRITE setRankFrom)
        Q_PROPERTY(float ratingTo READ ratingTo WRITE setRankTo)

    public:
        PhotosModelControllerComponent(QObject * = nullptr);

        void setDatabase(Database::IDatabase *);
        void setCompleterFactory(ICompleterFactory* completerFactory);

        // various getters
        APhotoInfoModel* model() const;
        QStringList categories() const;
        unsigned int datesCount() const;

        // getters for filter
        unsigned int timeViewFrom() const;
        unsigned int timeViewTo() const;
        QString searchExpression() const;
        bool newPhotosOnly() const;
        int category() const;
        float ratingFrom() const;
        float ratingTo() const;

        // setters for filters
        void setTimeViewFrom(unsigned int);
        void setTimeViewTo(unsigned int);
        void setSearchExpression(const QString &);
        void setNewPhotosOnly(bool);
        void setCategory(int);
        void setRankFrom(float);
        void setRankTo(float);

        // helpers and actions for qml
        Q_INVOKABLE QDate dateFor(unsigned int) const;
        Q_INVOKABLE void markNewAsReviewed();

    signals:
        void categoriesChanged() const;
        void datesCountChanged() const;

    private:
        QTimer m_searchLauncher;
        std::vector<QDate> m_dates;

        // filters
        QPair<unsigned int, unsigned int> m_timeView;
        QString m_searchExpression;
        QString m_categoryFilter;
        int m_ratingFrom = 0;
        int m_ratingTo = 10;
        bool m_newPhotosOnly;
        //

        FlatModel* m_model;
        Database::IDatabase* m_db;
        ICompleterFactory* m_completerFactory;

        void clear();
        void updateModelFilters();
        void setAvailableDates(const std::vector<TagValue> &);
        void updateTimeRange();
        Database::Filter allFilters() const;
        QStringList rawCategories() const;

        void getTimeRangeForFilters(Database::IBackend &);
        void markPhotosAsReviewed(Database::IBackend &);
};

#endif // PHOTOSMODELCOMPONENT_HPP
