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

#include <OpenLibrary/putils/ts_resource.hpp>

#include <core/base_tags.hpp>
#include <database/iphoto_info.hpp>

#include "idxdata_deepfetcher.hpp"
#include "photos_matcher.hpp"

namespace
{

    struct GetPhotosTask: Database::IGetPhotosTask
    {
        GetPhotosTask(ITasksResults* tr, const QModelIndex& parent): m_tasks_result(tr), m_parent(parent) {}
        GetPhotosTask(const GetPhotosTask &) = delete;
        virtual ~GetPhotosTask() {}

        GetPhotosTask& operator=(const GetPhotosTask &) = delete;

        virtual void got(const IPhotoInfo::List& photos)
        {
            m_tasks_result->got_getPhotos(this, photos);
        }

        ITasksResults* m_tasks_result;
        QModelIndex m_parent;
    };

    struct ListTagValuesTask: Database::IListTagValuesTask
    {
        ListTagValuesTask(ITasksResults* tr, const QModelIndex& parent, size_t level): m_tasks_result(tr), m_parent(parent), m_level(level) {}
        ListTagValuesTask(const ListTagValuesTask &) = delete;
        virtual ~ListTagValuesTask() {}

        ListTagValuesTask& operator=(const ListTagValuesTask &) = delete;

        virtual void got(const TagValue& values)
        {
            m_tasks_result->got_listTagValues(this, values);
        }

        ITasksResults* m_tasks_result;
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
        m_photoId2IdxData(),
        m_notFetchedIdxData(),
        m_mainThreadId(std::this_thread::get_id()),
        m_taskExecutor(nullptr)
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

    DATABASE_DEPRECATED DBDataModel* m_model;
    IdxData* m_root;
    Hierarchy m_hierarchy;
    Database::IDatabase* m_database;
    ol::ThreadSafeResource<std::unordered_map<IPhotoInfo::Id, IdxData *, PhotoInfoIdHash>> m_photoId2IdxData;
    ol::ThreadSafeResource<std::unordered_set<IdxData *>> m_notFetchedIdxData;
    std::thread::id m_mainThreadId;
    ITaskExecutor* m_taskExecutor;
};


IdxDataManager::IdxDataManager(DBDataModel* model): m_data(new Data(model))
{
    m_data->init(this);

    //default hierarchy
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


const Hierarchy& IdxDataManager::getHierarchy() const
{
    return m_data->m_hierarchy;
}


void IdxDataManager::set(ITaskExecutor* taskExecutor)
{
    m_data->m_taskExecutor = taskExecutor;
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

        m_data->m_taskExecutor->add(std::shared_ptr<IdxDataDeepFetcher>(fetcher));
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

    resetModel();
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

    if (idxData->m_photo)
        m_data->m_photoId2IdxData.lock()->insert( std::make_pair(idxData->m_photo->getID(), idxData) );
}


void IdxDataManager::idxDataDeleted(IdxData* idxData)
{
    removeIdxDataFromNotFetched(idxData);

    if (idxData->m_photo)
        m_data->m_photoId2IdxData.lock()->erase(idxData->m_photo->getID());
}


void IdxDataManager::idxDataReset(IdxData* idxData)
{
    addIdxDataToNotFetched(idxData);
}


//function returns list of tags on particular 'level' for 'parent'
void IdxDataManager::fetchTagValuesFor(size_t level, const QModelIndex& _parent)
{
    if (level + 1 <= m_data->m_hierarchy.levels.size())
    {
        std::deque<Database::IFilter::Ptr> filter;

        const TagNameInfo& tagNameInfo = m_data->m_hierarchy.levels[level].tagName;
        buildFilterFor(_parent, &filter);
        buildExtraFilters(&filter);

        std::unique_ptr<Database::IListTagValuesTask> task(new ListTagValuesTask(this, _parent, level));
        m_data->m_database->exec(std::move(task), tagNameInfo, filter);
    }
    else
        assert(!"should not happend");
}


void IdxDataManager::fetchPhotosFor(const QModelIndex& _parent)
{
    std::deque<Database::IFilter::Ptr> filter;
    buildFilterFor(_parent, &filter);
    buildExtraFilters(&filter);

    //prepare task and store it in local list
    std::unique_ptr<Database::IGetPhotosTask> task(new GetPhotosTask(this, _parent));

    //send task to execution
    m_data->m_database->exec(std::move(task), filter);
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

    if (level < m_data->m_hierarchy.levels.size())        //construct nodes basing on tags
        fetchTagValuesFor(level, _parent);
    else if (level == m_data->m_hierarchy.levels.size())   //construct leafs basing on photos
        fetchPhotosFor(_parent);
    else
        assert(!"should not happen");

    idxData->m_loaded = IdxData::FetchStatus::Fetching;
}


//called when leafs for particual node have been loaded
void IdxDataManager::got_getPhotos(Database::IGetPhotosTask* task, const IPhotoInfo::List& photos)
{
    GetPhotosTask* l_task = static_cast<GetPhotosTask *>(task);
    IdxData* parentIdxData = getParentIdxDataFor(l_task->m_parent);

    std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

    for(IPhotoInfo::Ptr photoInfo: photos)
    {
        IdxData* newItem = new IdxData(this, parentIdxData, photoInfo);
        leafs->push_back(newItem);
    }

    //attach nodes to parent node in main thread
    emit nodesFetched(parentIdxData, leafs);
}


//called when nodes for particual node have been loaded
void IdxDataManager::got_listTagValues(Database::IListTagValuesTask* task, const TagValue& tags)
{
    ListTagValuesTask* l_task = static_cast<ListTagValuesTask *>(task);

    const size_t level = l_task->m_level;
    const QModelIndex& _parent = l_task->m_parent;
    IdxData* parentIdxData = getParentIdxDataFor(_parent);

    std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

    for(const QString& tag: tags)
    {
        auto filter = std::make_shared<Database::FilterPhotosWithTag>();
        filter->tagName = m_data->m_hierarchy.levels[level].tagName;
        filter->tagValue = tag;

        IdxData* newItem = new IdxData(m_data->m_root->m_model, parentIdxData, tag);
        newItem->setNodeFilter(filter);

        leafs->push_back(newItem);
    }

    //attach nodes to parent node in main thread
    emit nodesFetched(parentIdxData, leafs);
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

    m_data->m_model->beginResetModel();
    m_data->m_root->reset();
    m_data->m_model->endResetModel();
}


void IdxDataManager::appendIdxData(IdxData* _parent, const std::deque<IdxData *>& photos)
{
    assert(photos.empty() == false);

    const QModelIndex parentIdx = m_data->m_model->createIndex(_parent);
    const size_t last = photos.size() - 1;
    m_data->m_model->beginInsertRows(parentIdx, 0, last);

    for(IdxData* newItem: photos)
        _parent->addChild(newItem);

    m_data->m_model->endInsertRows();
}


bool IdxDataManager::movePhotoToRightParent(const IPhotoInfo::Ptr& photoInfo)
{
    IdxData* currentParent = getCurrentParent(photoInfo);
    IdxData* newParent = createAncestry(photoInfo);
    bool parent_changed = currentParent != newParent;

    if (parent_changed)
    {
        if (currentParent == nullptr)
            performAdd(photoInfo, newParent);
        else if (newParent == nullptr)
            performRemove(photoInfo);
        else
            performMove(photoInfo, currentParent, newParent);
    }

    return parent_changed;
}


IdxData* IdxDataManager::getCurrentParent(const IPhotoInfo::Ptr& photoInfo)
{
    IdxData* item = findIdxDataFor(photoInfo);
    IdxData* result = item != nullptr? item->m_parent: nullptr;

    return result;
}


IdxData* IdxDataManager::createAncestry(const IPhotoInfo::Ptr& photoInfo)
{
    PhotosMatcher matcher;
    matcher.set(this);
    matcher.set(m_data->m_model);
    IdxData* _parent = nullptr;

    const bool match = matcher.doesMatchModelFilters(photoInfo);

    if (match)
    {
        _parent = matcher.findParentFor(photoInfo);

        //could not match exact parent?
        if (_parent == nullptr)
            _parent = createCloserAncestor(&matcher, photoInfo);

        //parent fetched? Attach photoInfo
        if (_parent != nullptr)
        {
            if (_parent->m_loaded != IdxData::FetchStatus::Fetched)
            {
                //Ancestor of photo isn't yet fetched. Don't fetch it. We will do it on user's demand
                //Just make sure we will return _parent == nullptr as we didn't achieve direct parent
                _parent = nullptr;
            }
        }
        else   // we could not find or create any reasonable parent. Create or attach to universal parent for such a orphans
            createUniversalAncestor(&matcher, photoInfo);
    }

    return _parent;
}


IdxData* IdxDataManager::findIdxDataFor(const IPhotoInfo::Ptr& photoInfo)
{
    const IPhotoInfo::Id id = photoInfo->getID();
    auto photosMap = m_data->m_photoId2IdxData.lock();
    auto it = photosMap->find(id);
    IdxData* result = nullptr;

    if (it != photosMap->end())
        result = it->second;

    return result;
}


IdxData *IdxDataManager::createCloserAncestor(PhotosMatcher* matcher, const IPhotoInfo::Ptr& photoInfo)
{
    IdxData* _parent = matcher->findCloserAncestorFor(photoInfo);
    IdxData* result = nullptr;

    const Tag::TagsList& photoTags = photoInfo->getTags();
    const size_t level = _parent->m_level;

    if (level == m_data->m_hierarchy.levels.size())  //this parent is at the bottom of hierarchy? Just use it as result
        result = _parent;
    else
    {
        const TagNameInfo& tagName = m_data->m_hierarchy.levels[level].tagName;
        auto photoTagIt = photoTags.find(tagName);

        //we need to add subnode for '_parent' we are sure it doesn't exist as 'createRightParent' takes closer ancestor for '_parent'
        if (photoTagIt != photoTags.end())
        {
            const auto tagValue = *photoTagIt->second.begin();
            IdxData* node = new IdxData(this, _parent, tagValue);

            auto filter = std::make_shared<Database::FilterPhotosWithTag>();
            filter->tagName = tagName;
            filter->tagValue = tagValue;

            node->setNodeFilter(filter);

            appendIdxData(_parent, {node} );

            result = node;
        }
        else
        {
            //photo doesn't match. It may be not loaded yet or has incomplete tags list
        }
    }

    return result;
}


IdxData* IdxDataManager::createUniversalAncestor(PhotosMatcher* matcher, const IPhotoInfo::Ptr& photoInfo)
{
    IdxData* _parent = matcher->findCloserAncestorFor(photoInfo);
    IdxData* node = new IdxData(this, _parent, tr("Nonmatching"));

    const size_t level = _parent->m_level;
    const TagNameInfo& tagName = m_data->m_hierarchy.levels[level].tagName;

    auto filter = std::make_shared<Database::FilterPhotosWithoutTag>();
    filter->tagName = tagName;

    node->setNodeFilter(filter);

    appendIdxData(_parent, {node} );

    return node;
}


void IdxDataManager::performMove(const IPhotoInfo::Ptr& photoInfo, IdxData* from, IdxData* to)
{
    IdxData* photoIdxData = findIdxDataFor(photoInfo);
    QModelIndex fromIdx = getIndex(from);
    QModelIndex toIdx = getIndex(to);
    const int fromPos = photoIdxData->m_row;
    const int toPos = to->m_children.size();

    m_data->m_model->beginMoveRows(fromIdx, fromPos, fromPos, toIdx, toPos);

    from->takeChild(photoIdxData);
    to->addChild(photoIdxData);

    m_data->m_model->endMoveRows();

    //remove empty parents
    if (from->m_children.empty())
        performRemove(from);
}


void IdxDataManager::performRemove(const IPhotoInfo::Ptr& photoInfo)
{
    IdxData* photoIdxData = findIdxDataFor(photoInfo);

    performRemove(photoIdxData);
}


void IdxDataManager::performRemove(IdxData* item)
{
    IdxData* _parent = item->m_parent;
    assert(_parent != nullptr);

    QModelIndex parentIdx = getIndex(_parent);
    const int itemPos = item->m_row;

    m_data->m_model->beginRemoveRows(parentIdx, itemPos, itemPos);

    _parent->removeChild(item);

    m_data->m_model->endRemoveRows();

    //remove empty parents
    if (_parent->m_children.empty())
        performRemove(_parent);
}


void IdxDataManager::performAdd(const IPhotoInfo::Ptr& photoInfo, IdxData* to)
{
    IdxData* photoIdxData = findIdxDataFor(photoInfo);
    QModelIndex toIdx = getIndex(to);
    const int toPos = to->m_children.size();

    m_data->m_model->beginInsertRows(toIdx, toPos, toPos);

    if (photoIdxData == nullptr)
        photoIdxData = new IdxData(this, to, photoInfo);

    to->addChild(photoIdxData);

    m_data->m_model->endInsertRows();
}


void IdxDataManager::insertFetchedNodes(IdxData* _parent, const std::shared_ptr<std::deque<IdxData *>>& nodes)
{
    //attach nodes to parent in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    if (nodes->empty() == false)
        appendIdxData(_parent, *nodes.get());

    markIdxDataFetched(_parent);
}


void IdxDataManager::photoChanged(const IPhotoInfo::Ptr& photoInfo)
{
    PhotosMatcher matcher;
    matcher.set(this);
    matcher.set(m_data->m_model);

    const bool match = matcher.doesMatchModelFilters(photoInfo);

    if (match)
    {
        //make sure photo is assigned to right parent
        movePhotoToRightParent(photoInfo);

        //tak IdxData now, it is possible we have removed it while moving to new parent
        IdxData* idx = findIdxDataFor(photoInfo);
        if (idx != nullptr)
        {
            QModelIndex index = getIndex(idx);

            emit m_data->m_model->dataChanged(index, index);
        }
    }
    else // photo doesn't match filters, but maybe it did?
    {
        IdxData* idx = findIdxDataFor(photoInfo);
        if (idx != nullptr)
            movePhotoToRightParent(photoInfo);
    }
}


void IdxDataManager::photoAdded(const IPhotoInfo::Ptr& photoInfo)
{
    PhotosMatcher matcher;
    matcher.set(this);
    matcher.set(m_data->m_model);

    const bool match = matcher.doesMatchModelFilters(photoInfo);

    if (match)
        movePhotoToRightParent(photoInfo);
}
