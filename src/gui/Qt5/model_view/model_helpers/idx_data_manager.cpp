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

#include "idx_data_manager.hpp"

#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <iostream>

#include <QModelIndex>
#include <QEventLoop>

#include <OpenLibrary/palgorithm/ts_resource.hpp>

#include <core/base_tags.hpp>

#include "idxdata_deepfetcher.hpp"

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


struct IdxDataManager::Data
{
    Data(DBDataModel* model):
        m_model(model),
        m_root(nullptr),
        m_hierarchy(),
        m_database(),
        m_iterator(),
        m_db_tasks(),
        m_notFetchedIdxData(),
        m_mainThreadId(std::this_thread::get_id())
    {
    }

    ~Data()
    {
        //delete manualy so `this` is still valid when notifications from deleted IdxData will come
        delete m_root;
    }

    void init(IdxDataManager* manager)
    {
        assert(m_root == nullptr);
        m_root = new IdxData(manager, nullptr, "");
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    DBDataModel* m_model;
    IdxData* m_root;
    Hierarchy m_hierarchy;
    Database::IDatabase* m_database;
    Database::PhotoIterator m_iterator;
    ThreadSafeResource<std::unordered_map<Database::Task, std::unique_ptr<ITaskData>, DatabaseTaskHash>> m_db_tasks;
    ThreadSafeResource<std::unordered_set<IdxData *>> m_notFetchedIdxData;
    std::thread::id m_mainThreadId;
};


IdxDataManager::IdxDataManager(DBDataModel* model): m_data(new Data(model))
{
    m_data->init(this);

    Hierarchy hierarchy;
    hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

    setHierarchy(hierarchy);
            
    qRegisterMetaType< std::shared_ptr<std::deque<IdxData *>> >("std::shared_ptr<std::deque<IdxData *>>");
    qRegisterMetaType<IPhotoInfo::Ptr>("IPhotoInfo::Ptr");
    
    //used for transferring event from working thread to main one
    connect(this, SIGNAL(nodesFetched(IdxData*, std::shared_ptr<std::deque<IdxData*> >)),
            this, SLOT(insertFetchedNodes(IdxData*, std::shared_ptr<std::deque<IdxData *> >)), Qt::QueuedConnection);
}


IdxDataManager::~IdxDataManager() {}


void IdxDataManager::setHierarchy(const Hierarchy& hierarchy)
{
    m_data->m_hierarchy = hierarchy;

    resetModel();
}


void IdxDataManager::fetchMore(const QModelIndex& _parent)
{
    fetchData(_parent);
}


void IdxDataManager::deepFetch(IdxData* top)
{
    if (m_data->m_notFetchedIdxData.lock().get().empty() == false)
    {
        IdxDataDeepFetcher* fetcher = new IdxDataDeepFetcher;
        fetcher->setModelImpl(this);
        fetcher->setIdxDataToFetch(top);

        //wait for this particular task to finish in event loop
        QEventLoop eventLoop;
        QEventLoopLocker* eventLoopLocker = new QEventLoopLocker(&eventLoop);
        fetcher->setEventLoopLocker(eventLoopLocker);

        TaskExecutorConstructor::get()->add(std::shared_ptr<IdxDataDeepFetcher>(fetcher));
        eventLoop.exec();
    }
}


bool IdxDataManager::canFetchMore(const QModelIndex& _parent)
{
    IdxData* idxData = getParentIdxDataFor(_parent);
    const bool status = idxData->m_loaded == IdxData::FetchStatus::NotFetched;

    return status;
}


void IdxDataManager::setDatabase(Database::IDatabase* database)
{
    if (m_data->m_database != nullptr)
        disconnect(m_data->m_database->notifier());

    m_data->m_database = database;

    connect(m_data->m_database->notifier(), SIGNAL(photoModified(IPhotoInfo::Ptr)), this, SLOT(photoChanged(IPhotoInfo::Ptr)));
    connect(m_data->m_database->notifier(), SIGNAL(photoAdded(IPhotoInfo::Ptr)),    this, SLOT(photoAdded(IPhotoInfo::Ptr)));
}


void IdxDataManager::close()
{
    if (m_data->m_database)
        m_data->m_database->closeConnections();
}


IdxData* IdxDataManager::getRoot()
{
    return m_data->m_root;
}


IdxData* IdxDataManager::getIdxDataFor(const QModelIndex& obj) const
{
    IdxData* idxData = static_cast<IdxData *>(obj.internalPointer());

    return idxData;
}


IdxData* IdxDataManager::getParentIdxDataFor(const QModelIndex& _parent)
{
    IdxData* idxData = getIdxDataFor(_parent);

    if (idxData == nullptr)
        idxData = m_data->m_root;

    return idxData;
}


QModelIndex IdxDataManager::getIndex(IdxData* idxData) const
{
    return m_data->m_model->createIndex(idxData);
}


bool IdxDataManager::hasChildren(const QModelIndex& _parent)
{
    // Always return true for unloaded nodes.
    // This prevents view from calling rowCount() before canFetchMore()

    bool status = false;
    IdxData* idxData = getParentIdxDataFor(_parent);

    if (idxData->m_loaded != IdxData::FetchStatus::Fetched)
        status = true;              //data not loaded assume there is something
    else
        status = !idxData->m_photo; //return true for nodes only, not for leafs

    return status;
}


IdxData* IdxDataManager::parent(const QModelIndex& child)
{
    IdxData* idxData = getIdxDataFor(child);
    IdxData* result  = idxData->m_parent;

    return result;
}


void IdxDataManager::getPhotosFor(const IdxData* idx, std::vector<IPhotoInfo::Ptr>* result)
{
    forIndexChildren(idx, [&] (const IdxData* child)
    {
        if (child->m_loaded == IdxData::FetchStatus::Fetched)
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


void IdxDataManager::idxDataCreated(IdxData* idxData)
{
    addIdxDataToNotFetched(idxData);
}


void IdxDataManager::idxDataDeleted(IdxData* idxData)
{
    removeIdxDataFromNotFetched(idxData);
}


void IdxDataManager::idxDataReset(IdxData* idxData)
{
    addIdxDataToNotFetched(idxData);
}


//function returns list of tags on particular 'level' for 'parent'
void IdxDataManager::getLevelInfo(size_t level, const QModelIndex& _parent)
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


void IdxDataManager::buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter)
{
    IdxData* idxData = getParentIdxDataFor(_parent);

    filter->push_back(idxData->m_filter);

    if (idxData->m_level > 0)
        buildFilterFor(_parent.parent(), filter);
}


void IdxDataManager::buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const
{
    const auto modelSpecificFilters = m_data->m_model->getModelSpecificFilters();
    filter->insert(filter->end(), modelSpecificFilters.begin(), modelSpecificFilters.end());
}


void IdxDataManager::fetchData(const QModelIndex& _parent)
{
    IdxData* idxData = getParentIdxDataFor(_parent);
    const size_t level = idxData->m_level;

    assert(idxData->m_loaded == IdxData::FetchStatus::NotFetched);

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

        idxData->m_loaded = IdxData::FetchStatus::Fetching;
}


void IdxDataManager::got_getAllPhotos(const Database::Task &, const Database::QueryList &)
{
}


void IdxDataManager::got_getPhoto(const Database::Task &, const IPhotoInfo::Ptr &)
{
}


//called when leafs for particual node have been loaded
void IdxDataManager::got_getPhotos(const Database::Task& task, const Database::QueryList& photos)
{
    auto tasks = m_data->m_db_tasks.lock();
    auto it = tasks->find(task);

    if (it != tasks->end())
    {
        GetPhotosTask* l_task = static_cast<GetPhotosTask *>(it->second.get());
        IdxData* parentIdxData = getParentIdxDataFor(l_task->m_parent);

        std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

        for(IPhotoInfo::Ptr photoInfo: photos)
        {
            IdxData* newItem = new IdxData(this, parentIdxData, photoInfo);
            leafs->push_back(newItem);
        }

        tasks->erase(it);

        //attach nodes to parent node in main thread
        emit nodesFetched(parentIdxData, leafs);
    }
}


void IdxDataManager::got_listTags(const Database::Task &, const std::vector<TagNameInfo> &)
{
}


//called when nodes for particual node have been loaded
void IdxDataManager::got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags)
{
    auto tasks = m_data->m_db_tasks.lock();
    auto it = tasks->find(task);

    //task may be invalid for example when model become invalid while task was executed
    if (it != tasks->end())
    {
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

            IdxData* newItem = new IdxData(m_data->m_root->m_model, parentIdxData, tag);
            newItem->setNodeData(fdesc);

            leafs->push_back(newItem);
        }

        tasks->erase(it);

        //attach nodes to parent node in main thread
        emit nodesFetched(parentIdxData, leafs);
    }
}


void IdxDataManager::got_storeStatus(const Database::Task &)
{
    //TODO: some validation?
}


void IdxDataManager::markIdxDataFetched(IdxData* idxData)
{
    idxData->m_loaded = IdxData::FetchStatus::Fetched;
    removeIdxDataFromNotFetched(idxData);
    emit idxDataLoaded(idxData);
}


void IdxDataManager::removeIdxDataFromNotFetched(IdxData* idxData)
{
    m_data->m_notFetchedIdxData.lock()->erase(idxData);
}


void IdxDataManager::addIdxDataToNotFetched(IdxData* idxData)
{
    m_data->m_notFetchedIdxData.lock()->insert(idxData);
}


void IdxDataManager::resetModel()
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    //mark all pending db tasks (if any) invalid as for now they reffer to previous state
    m_data->m_db_tasks.lock().get().clear();

    m_data->m_model->beginResetModel();
    m_data->m_root->reset();
    m_data->m_model->endResetModel();
}


void IdxDataManager::appendPhotos(IdxData* _parent, const std::deque< IdxData* >& photos)
{
    QModelIndex parentIdx = m_data->m_model->createIndex(_parent);
    const size_t last = photos.size() - 1;
    m_data->m_model->beginInsertRows(parentIdx, 0, last);

    for(IdxData* newItem: photos)
        _parent->addChild(newItem);

    m_data->m_model->endInsertRows();
}


void IdxDataManager::insertFetchedNodes(IdxData* _parent, const std::shared_ptr<std::deque<IdxData *>>& photos)
{
    //attach nodes to parent in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    appendPhotos(_parent, *photos.get());

    markIdxDataFetched(_parent);
}


void IdxDataManager::photoChanged(const IPhotoInfo::Ptr &)
{
    //TODO: not very smart. Do analyse what changed and how basing on photo id

    //resetModel();

    //QModelIndex idx = m_data->m_model->createIndex(idxData);
    //emit m_data->m_model->dataChanged(idx, idx);
}


void IdxDataManager::photoAdded(const IPhotoInfo::Ptr& photoInfo)
{
    //TODO: check if we are interested in this photo (does it match our filters?)

    IdxData* root = m_data->m_model->getRootIdxData();
    IdxData* newItem = new IdxData(this, root, photoInfo);

    std::deque<IdxData *> nodes = { newItem };
    appendPhotos(root, nodes);
}
