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

struct DBDataModelImpl: Database::IDatabaseClient
{
    struct DatabaseTaskHash
    {
        std::size_t operator()(const Database::Task& t) const
        {
            return t.getId();
        }
    };

    DBDataModelImpl(DBDataModel* model);
    DBDataModelImpl(const DBDataModelImpl &) = delete;
    DBDataModelImpl& operator=(const DBDataModelImpl &) = delete;

    ~DBDataModelImpl();

    void setHierarchy(const Hierarchy& hierarchy);
    bool isDirty() const;

    void fetchMore(const QModelIndex& _parent);
    void deepFetch(const IdxData* top);
    bool canFetchMore(const QModelIndex& _parent);
    void setBackend(Database::IDatabase* database);
    void close();

    IdxData* getRoot();
    IdxData* getIdxDataFor(const QModelIndex& obj) const;
    IdxData* getParentIdxDataFor(const QModelIndex& _parent);
    bool hasChildren(const QModelIndex& _parent);
    IdxData* parent(const QModelIndex& child);
    void addPhoto(const PhotoInfo::Ptr& photo);
    void getPhotosFor(const IdxData* idx, std::vector<PhotoInfo::Ptr>* result);

    //store or update photo in DB
    void updatePhotoInDB(const PhotoInfo::Ptr& photoInfo);


private:
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
    void fetchData(const QModelIndex& _parent, Database::IDatabase* database);
    virtual void got_getAllPhotos(const Database::Task &, const Database::QueryList &) override;
    virtual void got_getPhoto(const Database::Task &, const PhotoInfo::Ptr &) override;
    virtual void got_getPhotos(const Database::Task & task, const Database::QueryList& photos) override;
    virtual void got_listTags(const Database::Task &, const std::vector<TagNameInfo> &) override;
    virtual void got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags) override;
    virtual void got_storeStatus(const Database::Task &) override;
};


#endif // DBDATAMODELIMPL_H
