/*
 * DBDataModel's private implementation
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

#ifndef DBDATAMODELIMPL_H
#define DBDATAMODELIMPL_H

#include <database/idatabase.hpp>

#include "idx_data.hpp"
#include "../db_data_model.hpp"

class DBDataModel;

struct IdxDataManager: QObject, Database::IDatabaseClient
{
    struct DatabaseTaskHash
    {
        std::size_t operator()(const Database::Task& t) const
        {
            return t.getId();
        }
    };

    IdxDataManager(DBDataModel* model);
    IdxDataManager(const IdxDataManager &) = delete;
    IdxDataManager& operator=(const IdxDataManager &) = delete;

    ~IdxDataManager();

    void setHierarchy(const Hierarchy& hierarchy);

    void fetchMore(const QModelIndex& _parent);
    void deepFetch(IdxData* top);
    bool canFetchMore(const QModelIndex& _parent);
    void setBackend(Database::IDatabase* database);
    void close();

    IdxData* getRoot();
    IdxData* getIdxDataFor(const QModelIndex& obj) const;
    IdxData* getParentIdxDataFor(const QModelIndex& _parent);
    QModelIndex getIndex(IdxData* idxData) const;
    bool hasChildren(const QModelIndex& _parent);
    IdxData* parent(const QModelIndex& child);
    void getPhotosFor(const IdxData* idx, std::vector<PhotoInfo::Ptr>* result);

    //store or update photo in DB
    void updatePhotoInDB(const PhotoInfo::Ptr& photoInfo);

    void idxDataChanged(IdxData *);                              //called by IdxData when it changes

    //signals from IdxData:
    void idxDataCreated(IdxData *);
    void idxDataDeleted(IdxData *);
    void idxDataReset(IdxData *);

private:
    Q_OBJECT
    
    struct Data;
    std::unique_ptr<Data> m_data;

    template<typename T>
    void forIndexChildren(const IdxData* index, const T& action)
    {
        for(const IdxData* child: index->m_children)
            action(child);
    }

    //function returns list of tags on particular 'level' for 'parent'
    void getLevelInfo(size_t level, const QModelIndex& _parent);
    void buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter);
    void buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const;
    void fetchData(const QModelIndex &);

    //Database::IDatabaseClient:
    virtual void got_getAllPhotos(const Database::Task &, const Database::QueryList &) override;
    virtual void got_getPhoto(const Database::Task &, const PhotoInfo::Ptr &) override;
    virtual void got_getPhotos(const Database::Task & task, const Database::QueryList& photos) override;
    virtual void got_listTags(const Database::Task &, const std::vector<TagNameInfo> &) override;
    virtual void got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags) override;
    virtual void got_storeStatus(const Database::Task &) override;

    void markIdxDataFetched(IdxData *);

    void removeIdxDataFromNotFetched(IdxData *);
    void addIdxDataToNotFetched(IdxData *);

    void resetModel();

signals:
    void idxDataLoaded(IdxData *);
    void nodesFetched(IdxData *, const std::shared_ptr<std::deque<IdxData *>> &);
    
private slots:
    void insertFetchedNodes(IdxData *, const std::shared_ptr<std::deque<IdxData *>> &);
    void photoChanged(const PhotoInfo::Ptr &);
    void photoAdded(const PhotoInfo::Ptr &);
};


#endif // DBDATAMODELIMPL_H
