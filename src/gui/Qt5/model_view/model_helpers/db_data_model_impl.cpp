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

#include "db_data_model_impl.hpp"

#include <unordered_map>

#include <QModelIndex>

#include <OpenLibrary/palgorithm/ts_resource.hpp>

#include <core/base_tags.hpp>


namespace
{
    struct ITaskData
    {
        virtual ~ITaskData() {}
    };

    struct GetPhotosTask: ITaskData
    {
        GetPhotosTask(const QModelIndex& parent): m_parent(parent) {}
        virtual ~GetPhotosTask() {}

        QModelIndex m_parent;
    };

    struct ListTagValuesTask: ITaskData
    {
        ListTagValuesTask(const QModelIndex& parent, size_t level): m_parent(parent), m_level(level) {}
        virtual ~ListTagValuesTask() {}

        QModelIndex m_parent;
        size_t m_level;
    };
}


struct DBDataModelImpl::Data
{
    Data(DBDataModel* model):
        pThis(model),
        m_root(model, nullptr, ""),
        m_hierarchy(),
        m_dirty(true),
        m_database(),
        m_iterator(),
        m_db_tasks()
    {
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    DBDataModel* pThis;
    IdxData m_root;
    Hierarchy m_hierarchy;
    bool m_dirty;
    Database::IDatabase* m_database;
    Database::PhotoIterator m_iterator;
    ThreadSafeResource<std::unordered_map<Database::Task, std::unique_ptr<ITaskData>, DatabaseTaskHash>> m_db_tasks;
};


DBDataModelImpl::DBDataModelImpl(DBDataModel* model): m_data(new Data(model))
{
    Hierarchy hierarchy;
    hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

    setHierarchy(hierarchy);
}


DBDataModelImpl::~DBDataModelImpl() {}


void DBDataModelImpl::setHierarchy(const Hierarchy& hierarchy)
{
    m_data->m_hierarchy = hierarchy;
    m_data->m_dirty = true;
    m_data->m_root.reset();
}


bool DBDataModelImpl::isDirty() const
{
    return m_data->m_dirty;
}


void DBDataModelImpl::fetchMore(const QModelIndex& _parent)
{
    fetchData(_parent);
}


void DBDataModelImpl::deepFetch(const IdxData* top)
{
    forIndexChildren(top, [&](const IdxData* child)
    {
        if (child->m_loaded == IdxData::LoadStatus::NotLoaded)
        {
            if (child->m_photo.get() == nullptr)
                deepFetch(child);
        }
        else
        {
        }
    });
}


bool DBDataModelImpl::canFetchMore(const QModelIndex& _parent)
{
    IdxData* idxData = getParentIdxDataFor(_parent);
    const bool status = idxData->m_loaded == IdxData::LoadStatus::NotLoaded;

    return status;
}


void DBDataModelImpl::setBackend(Database::IDatabase* database)
{
    m_data->m_database = database;
}


void DBDataModelImpl::close()
{
    if (m_data->m_database)
        m_data->m_database->closeConnections();
}


IdxData* DBDataModelImpl::getRoot()
{
    return &m_data->m_root;
}


IdxData* DBDataModelImpl::getIdxDataFor(const QModelIndex& obj) const
{
    IdxData* idxData = static_cast<IdxData *>(obj.internalPointer());

    return idxData;
}


IdxData* DBDataModelImpl::getParentIdxDataFor(const QModelIndex& _parent)
{
    IdxData* idxData = getIdxDataFor(_parent);

    if (idxData == nullptr)
        idxData = &m_data->m_root;

    return idxData;
}


QModelIndex DBDataModelImpl::getIndex(IdxData* idxData) const
{
    return m_data->pThis->createIndex(idxData);
}


bool DBDataModelImpl::hasChildren(const QModelIndex& _parent)
{
    // Always return true for unloaded nodes.
    // This prevents view from calling rowCount() before canFetchMore()

    bool status = false;
    IdxData* idxData = getParentIdxDataFor(_parent);

    if (idxData->m_loaded != IdxData::LoadStatus::Loaded)
        status = true;              //data not loaded assume there is something
    else
        status = !idxData->m_photo; //return true for nodes only, not for leafs

    return status;
}


IdxData* DBDataModelImpl::parent(const QModelIndex& child)
{
    IdxData* idxData = getIdxDataFor(child);
    IdxData* result  = idxData->m_parent;

    return result;
}


void DBDataModelImpl::addPhoto(const PhotoInfo::Ptr& photo)
{
    m_data->m_root.addChild(photo);
}


void DBDataModelImpl::getPhotosFor(const IdxData* idx, std::vector<PhotoInfo::Ptr>* result)
{
    forIndexChildren(idx, [&] (const IdxData* child)
    {
        if (child->m_loaded == IdxData::LoadStatus::Loaded)
        {
            if (child->m_photo.get() == nullptr)
                getPhotosFor(child, result);
            else
                result->push_back(child->m_photo);
        }
        else
            assert(!"load not implemented");
    });
}


//store or update photo in DB
void DBDataModelImpl::updatePhotoInDB(const PhotoInfo::Ptr& photoInfo)
{
    if (photoInfo->isLoaded())
    {
        Database::Task task = m_data->m_database->prepareTask(this);
        m_data->m_database->store(task, photoInfo);
    }
}


//function returns list of tags on particular 'level' for 'parent'
void DBDataModelImpl::getLevelInfo(size_t level, const QModelIndex& _parent)
{
    if (level + 1 <= m_data->m_hierarchy.levels.size())
    {
        std::deque<Database::IFilter::Ptr> filter;

        const TagNameInfo& tagNameInfo = m_data->m_hierarchy.levels[level].tagName;
        buildFilterFor(_parent, &filter);
        buildExtraFilters(&filter);

        Database::Task task = m_data->m_database->prepareTask(this);
        m_data->m_db_tasks.lock().get()[task] = std::unique_ptr<ITaskData>(new ListTagValuesTask(_parent, level));
        m_data->m_database->listTagValues(task, tagNameInfo, filter);
    }
}


void DBDataModelImpl::buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter)
{
    IdxData* idxData = getParentIdxDataFor(_parent);

    filter->push_back(idxData->m_filter);

    if (idxData->m_level > 0)
        buildFilterFor(_parent.parent(), filter);
}


void DBDataModelImpl::buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const
{
    const auto modelSpecificFilters = m_data->m_root.m_model->getModelSpecificFilters();
    filter->insert(filter->end(), modelSpecificFilters.begin(), modelSpecificFilters.end());
}


void DBDataModelImpl::fetchData(const QModelIndex& _parent)
{
    IdxData* idxData = getParentIdxDataFor(_parent);
    const size_t level = idxData->m_level;

    if (level < m_data->m_hierarchy.levels.size())  //construct nodes basing on tags
        getLevelInfo(level, _parent);
    else
        if (level == m_data->m_hierarchy.levels.size())  //construct leafs basing on photos
        {
            std::deque<Database::IFilter::Ptr> filter;
            buildFilterFor(_parent, &filter);
            buildExtraFilters(&filter);

            //prepare task and store it in local list
            Database::Task task = m_data->m_database->prepareTask(this);
            m_data->m_db_tasks.lock().get()[task] = std::unique_ptr<ITaskData>(new GetPhotosTask(_parent));

            //send task to execution
            m_data->m_database->getPhotos(task, filter);
        }
        else
            assert(!"should not happen");

        idxData->m_loaded = IdxData::LoadStatus::Loading;
}


void DBDataModelImpl::got_getAllPhotos(const Database::Task &, const Database::QueryList &)
{
}


void DBDataModelImpl::got_getPhoto(const Database::Task &, const PhotoInfo::Ptr &)
{
}


//called when leafs for particual node have been loaded
void DBDataModelImpl::got_getPhotos(const Database::Task& task, const Database::QueryList& photos)
{
    auto it = m_data->m_db_tasks.lock().get().find(task);
    GetPhotosTask* l_task = static_cast<GetPhotosTask *>(it->second.get());
    IdxData* parentIdxData = getParentIdxDataFor(l_task->m_parent);

    std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

    for(PhotoInfo::Ptr photoInfo: photos)
    {
        IdxData* newItem = new IdxData(m_data->pThis, parentIdxData, photoInfo);
        leafs->push_back(newItem);
    }

    markIdxDataLoaded(parentIdxData);
    m_data->m_db_tasks.lock().get().erase(it);

    //attach photos to parent node in main thread
    m_data->pThis->attachNodes(parentIdxData, leafs);
}


void DBDataModelImpl::got_listTags(const Database::Task &, const std::vector<TagNameInfo> &)
{
}


//called when nodes for particual node have been loaded
void DBDataModelImpl::got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags)
{
    auto it = m_data->m_db_tasks.lock().get().find(task);
    ListTagValuesTask* l_task = static_cast<ListTagValuesTask *>(it->second.get());

    const size_t level = l_task->m_level;
    const QModelIndex& _parent = l_task->m_parent;
    IdxData* parentIdxData = getParentIdxDataFor(_parent);

    std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

    for(const TagValueInfo& tag: tags)
    {
        auto fdesc = std::make_shared<Database::FilterDescription>();
        fdesc->tagName = m_data->m_hierarchy.levels[level].tagName;
        fdesc->tagValue = tag;

        IdxData* newItem = new IdxData(m_data->m_root.m_model, parentIdxData, tag);
        newItem->setNodeData(fdesc);

        leafs->push_back(newItem);
    }

    markIdxDataLoaded(parentIdxData);
    m_data->m_db_tasks.lock().get().erase(it);

    //attach nodes to parent node in main thread
    m_data->pThis->attachNodes(parentIdxData, leafs);
}


void DBDataModelImpl::got_storeStatus(const Database::Task &)
{
    //TODO: some validation?
}


void DBDataModelImpl::markIdxDataLoaded(IdxData* idxData)
{
    idxData->m_loaded = IdxData::LoadStatus::Loaded;
    emit idxDataLoaded(idxData);
}

