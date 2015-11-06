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

//#include <database/query_list.hpp>
#include <database/filter.hpp>

#include "model_helpers/idx_data.hpp"
#include "model_helpers/idx_data_manager.hpp"


Hierarchy::Level::Level(): tagName(), order()
{

}


Hierarchy::Level::Level(const TagNameInfo& i, const Hierarchy::Level::Order& o): tagName(i), order(o)
{

}


Hierarchy::Hierarchy(): levels()
{

}


Hierarchy::Hierarchy(const std::initializer_list<Hierarchy::Level>& l): levels(l)
{

}


size_t Hierarchy::nodeLevels() const
{
    return levels.size() - 1;        // last level is for leafs description
}


const Hierarchy::Level& Hierarchy::getNodeInfo(size_t level) const
{
    assert(level < levels.size());   // less than real size of levels?

    return levels[level];
}


const Hierarchy::Level& Hierarchy::getLeafsInfo() const
{
    const size_t last = levels.size() - 1;

    return levels[last];
}


//////////////////////////////////////


DBDataModel::DBDataModel(QObject* p): AScalableImagesModel(p), m_idxDataManager(new IdxDataManager(this)), m_database(nullptr), m_filters()
{
}


DBDataModel::~DBDataModel()
{

}


void DBDataModel::setHierarchy(const Hierarchy& hierarchy)
{
    m_idxDataManager->setHierarchy(hierarchy);
}


void DBDataModel::deepFetch(const QModelIndex& top)
{
    IdxData* idx = m_idxDataManager->getParentIdxDataFor(top);
    m_idxDataManager->deepFetch(idx);
}


IPhotoInfo::Ptr DBDataModel::getPhoto(const QModelIndex& idx) const
{
    IdxData* idxData = m_idxDataManager->getIdxDataFor(idx);
    return idxData->m_photo;
}


const std::vector<IPhotoInfo::Ptr> DBDataModel::getPhotos()
{
    std::vector<IPhotoInfo::Ptr> result;
    m_idxDataManager->getPhotosFor(m_idxDataManager->getRoot(), &result);

    return result;
}


QImage DBDataModel::getImageFor(const QModelIndex& idx, const QSize& size)
{

}


bool DBDataModel::canFetchMore(const QModelIndex& _parent) const
{
    const bool status = m_database == nullptr? false: m_idxDataManager->canFetchMore(_parent);

    return status;
}


void DBDataModel::fetchMore(const QModelIndex& _parent)
{
    m_idxDataManager->fetchMore(_parent);
}


int DBDataModel::columnCount(const QModelIndex &) const
{
    return 1;
}


QVariant DBDataModel::data(const QModelIndex& _index, int role) const
{
    IdxData* idxData = m_idxDataManager->getIdxDataFor(_index);
    const QVariant& v = idxData->m_data[role];

    return v;
}


QModelIndex DBDataModel::index(int row, int column, const QModelIndex& _parent) const
{
    QModelIndex idx;
    IdxData* pData = m_idxDataManager->getParentIdxDataFor(_parent);

    if (static_cast<unsigned int>(row) < pData->m_children.size())   //row out boundary?
    {
        IdxData* cData = pData->m_children[row];
        idx = createIndex(row, column, cData);
    }

    return idx;
}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{
    IdxData* idxData = m_idxDataManager->parent(child);
    QModelIndex parentIdx = idxData? createIndex(idxData): QModelIndex();

    return parentIdx;
}


int DBDataModel::rowCount(const QModelIndex& _parent) const
{
    IdxData* idxData = m_idxDataManager->getParentIdxDataFor(_parent);
    const size_t count = idxData->m_children.size();

    return count;
}


bool DBDataModel::hasChildren(const QModelIndex& _parent) const
{
    return m_idxDataManager->hasChildren(_parent);
}


void DBDataModel::setDatabase(Database::IDatabase* database)
{
    m_idxDataManager->setDatabase(database);
    m_database = database;
}


void DBDataModel::set(ITaskExecutor* taskExecutor)
{
    m_idxDataManager->set(taskExecutor);
}


void DBDataModel::setPermanentFilters(const std::deque<Database::IFilter::Ptr>& filters)
{
    m_filters = filters;
}


const std::deque<Database::IFilter::Ptr>& DBDataModel::getPermanentFilters() const
{
    return m_filters;
}


bool DBDataModel::isEmpty() const
{
    IdxData* root = m_idxDataManager->getRoot();
    const bool result = root->m_children.empty();

    return result;
}


IdxData* DBDataModel::getRootIdxData()
{
    return m_idxDataManager->getRoot();
}


Database::IDatabase* DBDataModel::getDatabase()
{
    return m_database;
}


QModelIndex DBDataModel::createIndex(IdxData* idxData) const
{
    const QModelIndex idx = idxData->m_level == 0? QModelIndex():          //level 0 == parent of all parents represented by invalid index
                                                   createIndex(idxData->getRow(), idxData->getCol(), idxData);
    return idx;
}
