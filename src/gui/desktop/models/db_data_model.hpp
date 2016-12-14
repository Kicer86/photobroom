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

#ifndef DBDATAMODEL_HPP
#define DBDATAMODEL_HPP

#include <memory>

#include <database/idatabase.hpp>

#include "aphoto_info_model.hpp"
#include "model_types.hpp"

struct ITaskExecutor;
class IdxDataManager;
class IdxData;

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

        Level();
        Level(const TagNameInfo &, const Order &);
    };

    Hierarchy();
    Hierarchy(const std::initializer_list<Level> &);

    size_t nodeLevels() const;                       //number of node levels
    const Level& getNodeInfo(size_t level) const;
    const Level& getLeafsInfo() const;

    private:
        std::vector<Level> levels;
};


class DBDataModel: public APhotoInfoModel
{
        friend class IdxDataManager;

    public:
        enum Roles
        {
            NodeStatus = APhotoInfoModel::LastRole + 1,
        };

        DBDataModel(QObject* p = nullptr);
        ~DBDataModel();
        DBDataModel(const DBDataModel& other) = delete;
        DBDataModel& operator=(const DBDataModel& other) = delete;
        bool operator==(const DBDataModel& other) = delete;

        IPhotoInfo::Ptr getPhoto(const QModelIndex &) const;
        const std::vector<IPhotoInfo::Ptr> getPhotos() const;       //an empty result will be returned when any of nodes is not loaded. Use deepFetch() on main node to load all nodes
        const std::deque<Database::IFilter::Ptr>& getStaticFilters() const;
        bool isEmpty() const;

        void deepFetch(const QModelIndex &);                        //loads provided index and all its children recursively
        void setHierarchy(const Hierarchy &);
        void setDatabase(Database::IDatabase *);
        void set(ITaskExecutor *);
        void setStaticFilters(const std::deque<Database::IFilter::Ptr> &);
        void applyFilters(const SearchExpressionEvaluator::Expression &);

        // APhotoInfoModel:
        virtual IPhotoInfo* getPhotoInfo(const QModelIndex &) const override;

        // QAbstractItemModel:
        virtual bool canFetchMore(const QModelIndex& parent) const override;
        virtual void fetchMore(const QModelIndex& parent) override;

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& child) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

    protected:
        IdxData* getRootIdxData();

    private:
        std::unique_ptr<IdxDataManager> m_idxDataManager;
        Database::IDatabase* m_database;
        std::deque<Database::IFilter::Ptr> m_filters;

        using QAbstractItemModel::createIndex;
        QModelIndex createIndex(IdxData *) const;

        void itemDataChanged(IdxData *, const QVector<int> &);
};

#endif // DBDATAMODEL_HPP
