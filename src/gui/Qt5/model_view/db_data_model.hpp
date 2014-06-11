/*
 * Database based data model
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

#ifndef DBDATAMODEL_H
#define DBDATAMODEL_H

#include <memory>

#include <QAbstractItemModel>

#include <database/idatabase.hpp>

struct IdxData;

struct Hierarchy
{
    struct Level
    {
        TagNameInfo tagName;

        enum class Order
        {
            ascending,
            descending
        } order;
    };

    std::vector<Level> levels;

    Hierarchy(): levels() {}
};


class DBDataModel: public QAbstractItemModel, public Database::IFrontend
{
        Q_OBJECT

    public:
        DBDataModel(QObject* p);
        ~DBDataModel();
        DBDataModel(const DBDataModel& other) = delete;
        DBDataModel& operator=(const DBDataModel& other) = delete;
        bool operator==(const DBDataModel& other) = delete;

        void setHierarchy(const Hierarchy &);
        PhotoInfo::Ptr getPhoto(const QModelIndex &) const;
        const std::vector<PhotoInfo::Ptr> getPhotos();

        //Database::IFrontend:
        virtual void setBackend(Database::IBackend *) override;
        virtual void close() override;

    protected:
        IdxData& getRootIdxData();
        void updatePhotoInDB(const PhotoInfo::Ptr &);

        virtual std::vector<Database::IFilter::Ptr> getModelSpecificFilters() const = 0;

    private:
        friend struct IdxData;

        //QAbstractItemModel:
        virtual bool canFetchMore(const QModelIndex& parent) const override;
        virtual void fetchMore(const QModelIndex& parent) override;

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& child) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

        //own:
        using QAbstractItemModel::createIndex;
        QModelIndex createIndex(IdxData *) const;

        struct Impl;
        std::unique_ptr<Impl> m_impl;

        //used by friends
        void idxUpdated(IdxData *);

    private slots:
        void dispatchIdxUpdate(IdxData *);

    signals:
        void dispatchUpdate(IdxData *);
};

#endif // DBDATAMODEL_H
