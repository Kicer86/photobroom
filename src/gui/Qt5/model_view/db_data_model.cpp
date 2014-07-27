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

#include "db_data_model.hpp"

#include <unordered_map>
#include <memory>

#include <database/query_list.hpp>
#include <database/filter.hpp>

#include "model_helpers/idx_data.hpp"
#include "model_helpers/db_data_model_impl.hpp"


DBDataModel::DBDataModel(QObject* p): QAbstractItemModel(p), m_impl(new DBDataModelImpl(this))
{
    qRegisterMetaType< std::shared_ptr<std::deque<IdxData *>> >("std::shared_ptr<std::deque<IdxData *>>");

    //used for moving notifications to main thread
    connect(this, SIGNAL(s_idxUpdated(IdxData*)), this, SLOT(mt_idxUpdate(IdxData*)));
    connect(this, SIGNAL(s_attachNodes(IdxData*,std::shared_ptr<std::deque<IdxData*> >)),
            this, SLOT(mt_attachNodes(IdxData*,std::shared_ptr<std::deque<IdxData *> >)));
}


DBDataModel::~DBDataModel()
{

}


void DBDataModel::setHierarchy(const Hierarchy& hierarchy)
{
    beginResetModel();
    m_impl->setHierarchy(hierarchy);
    endResetModel();
}


void DBDataModel::deepFetch(const QModelIndex& top)
{
    IdxData* idx = m_impl->getParentIdxDataFor(top);
    m_impl->deepFetch(idx);
}


PhotoInfo::Ptr DBDataModel::getPhoto(const QModelIndex& idx) const
{
    IdxData* idxData = m_impl->getIdxDataFor(idx);
    return idxData->m_photo;
}


const std::vector<PhotoInfo::Ptr> DBDataModel::getPhotos()
{
    std::vector<PhotoInfo::Ptr> result;
    m_impl->getPhotosFor(m_impl->getRoot(), &result);

    return result;
}


bool DBDataModel::canFetchMore(const QModelIndex& _parent) const
{
    return m_impl->canFetchMore(_parent);
}


void DBDataModel::fetchMore(const QModelIndex& _parent)
{
    m_impl->fetchMore(_parent);
}


int DBDataModel::columnCount(const QModelIndex &) const
{
    return 1;
}


QVariant DBDataModel::data(const QModelIndex& _index, int role) const
{
    IdxData* idxData = m_impl->getIdxDataFor(_index);
    const QVariant& v = idxData->m_data[role];

    return v;
}


QModelIndex DBDataModel::index(int row, int column, const QModelIndex& _parent) const
{
    IdxData* pData = m_impl->getParentIdxDataFor(_parent);
    IdxData* cData = pData->m_children[row];
    QModelIndex idx = createIndex(row, column, cData);

    //check data consistency
    assert(cData->m_row == row);
    assert(cData->m_column == column);

    return idx;
}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{
    IdxData* idxData = m_impl->parent(child);
    QModelIndex parentIdx = idxData? createIndex(idxData): QModelIndex();

    return parentIdx;
}


int DBDataModel::rowCount(const QModelIndex& _parent) const
{
    IdxData* idxData = m_impl->getParentIdxDataFor(_parent);
    const size_t count = idxData->m_children.size();

    return count;
}


bool DBDataModel::hasChildren(const QModelIndex& _parent) const
{
    return m_impl->hasChildren(_parent);
}


void DBDataModel::setDatabase(Database::IDatabase* database)
{
    m_impl->setBackend(database);
}


void DBDataModel::close()
{
    m_impl->close();
}


void DBDataModel::idxUpdated(IdxData* idxData)
{
    //make sure, we will move to main thread
    emit s_idxUpdated(idxData);
}

void DBDataModel::attachNodes(IdxData* _parent, const std::shared_ptr<std::deque<IdxData *>>& leafs)
{
    //make sure, we will move to main thread
    emit s_attachNodes(_parent, leafs);
}


IdxData* DBDataModel::getRootIdxData()
{
    return m_impl->getRoot();
}


void DBDataModel::updatePhotoInDB(const PhotoInfo::Ptr& photoInfo)
{
    m_impl->updatePhotoInDB(photoInfo);
}


QModelIndex DBDataModel::createIndex(IdxData* idxData) const
{
    const QModelIndex idx = idxData->m_level == 0? QModelIndex():          //level 0 == parent of all parents represented by invalid index
                                                   createIndex(idxData->m_row, idxData->m_column, idxData);
    return idx;
}


void DBDataModel::mt_idxUpdate(IdxData* idxData)
{
    QModelIndex idx = createIndex(idxData);
    emit dataChanged(idx, idx);

    //if photo changed, store it in database
    PhotoInfo::Ptr photoInfo = idxData->m_photo;
    if (photoInfo.get() != nullptr)
        m_impl->updatePhotoInDB(photoInfo);
}


void DBDataModel::mt_attachNodes(IdxData* _parent, const std::shared_ptr<std::deque<IdxData *>>& photos)
{
    //attach nodes to parent in main thread
    QModelIndex parentIdx = createIndex(_parent);
    const size_t last = photos->size() - 1;
    beginInsertRows(parentIdx, 0, last);

    for(IdxData* newItem: *photos)
        _parent->addChild(newItem);

    endInsertRows();
}
