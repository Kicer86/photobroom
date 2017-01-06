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
#include <core/callback_ptr.hpp>
#include <database/iphoto_info.hpp>

#include "idxdata_deepfetcher.hpp"
#include "photos_matcher.hpp"


Q_DECLARE_METATYPE(std::shared_ptr<std::deque<IdxData *>>)
Q_DECLARE_METATYPE(std::deque<IPhotoInfo::Ptr>)


namespace
{

    struct GetPhotosTask: Database::AGetPhotosTask
    {
        GetPhotosTask(const std::function< void(Database::AGetPhotosTask* , const IPhotoInfo::List&)>& tr, const QModelIndex& parent): m_tasks_result(tr), m_parent(parent) {}
        GetPhotosTask(const GetPhotosTask &) = delete;
        virtual ~GetPhotosTask() {}

        GetPhotosTask& operator=(const GetPhotosTask &) = delete;

        virtual void got(const IPhotoInfo::List& photos)
        {
            m_tasks_result(this, photos);
        }

        std::function< void(Database::AGetPhotosTask* , const IPhotoInfo::List&)> m_tasks_result;
        QModelIndex m_parent;
    };

    struct GetNonmatchingPhotosTask: Database::AGetPhotosCount
    {
        GetNonmatchingPhotosTask(const std::function<void( Database::AGetPhotosCount *, int)>& tr, const QModelIndex& parent): m_tasks_result(tr), m_parent(parent) {}
        GetNonmatchingPhotosTask(const GetNonmatchingPhotosTask &) = delete;
        virtual ~GetNonmatchingPhotosTask() {}

        GetNonmatchingPhotosTask& operator=(const GetNonmatchingPhotosTask &) = delete;

        virtual void got(int size)
        {
            m_tasks_result(this, size);
        }

        std::function<void( Database::AGetPhotosCount *, int)> m_tasks_result;
        QModelIndex m_parent;
    };

    struct ListTagValuesTask: Database::AListTagValuesTask
    {
        ListTagValuesTask(const std::function<void(Database::AListTagValuesTask *, const std::deque<TagValue> &)>& tr,
                        const QModelIndex& parent,
                        size_t level): m_tasks_result(tr), m_parent(parent), m_level(level) {}
        ListTagValuesTask(const ListTagValuesTask &) = delete;
        virtual ~ListTagValuesTask() {}

        ListTagValuesTask& operator=(const ListTagValuesTask &) = delete;

        virtual void got(const std::deque<TagValue>& value)
        {
            m_tasks_result(this, value);
        }

        std::function<void(Database::AListTagValuesTask *, const std::deque<TagValue> &)> m_tasks_result;
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
        m_taskExecutor(nullptr),
        m_tasksResultsCtrl(),
        filterExpression()
    {
    }

    ~Data()
    {
        // disable any notifications from database
        m_tasksResultsCtrl.invalidate();

        delete m_root;
    }

    void init(IdxDataManager* manager)
    {
        assert(m_root == nullptr);
        m_root = new IdxData(manager, "");
        m_root->setParent(nullptr);
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    DBDataModel* m_model;
    IdxData* m_root;
    Hierarchy m_hierarchy;
    Database::IDatabase* m_database;
    ol::ThreadSafeResource<std::unordered_map<Photo::Id, IdxData *, Photo::IdHash>> m_photoId2IdxData;
    ol::ThreadSafeResource<std::unordered_set<IdxData *>> m_notFetchedIdxData;
    std::thread::id m_mainThreadId;
    ITaskExecutor* m_taskExecutor;
    safe_callback_ctrl m_tasksResultsCtrl;
    SearchExpressionEvaluator::Expression filterExpression;
};


IdxDataManager::INotifications::~INotifications()
{

}


IdxDataManager::IdxDataManager(DBDataModel* model): m_data(new Data(model))
{
    m_data->init(this);

    //default hierarchy
    const Hierarchy hierarchy = {
                                  { TagNameInfo(BaseTagsList::Date), Hierarchy::Level::Order::ascending },
                                  { TagNameInfo(BaseTagsList::Time), Hierarchy::Level::Order::ascending }
                                };

    setHierarchy(hierarchy);

    qRegisterMetaType< std::shared_ptr<std::deque<IdxData *>> >();
    qRegisterMetaType<IPhotoInfo::Ptr>("IPhotoInfo::Ptr");
    qRegisterMetaType<std::deque<IPhotoInfo::Ptr>>();
    qRegisterMetaType<Photo::Id>();
    qRegisterMetaType<std::deque<Photo::Id>>();

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
    if (m_data->m_database != nullptr)
        fetchData(_parent);
}


void IdxDataManager::deepFetch(IdxData* top)
{
    if (m_data->m_notFetchedIdxData.lock()->empty() == false)
    {
        IdxDataDeepFetcher* fetcher = new IdxDataDeepFetcher;
        fetcher->setModelImpl(this);
        fetcher->setIdxDataToFetch(top);

        //wait for this particular task to finish in event loop
        QEventLoop eventLoop;
        QEventLoopLocker* eventLoopLocker = new QEventLoopLocker(&eventLoop);
        fetcher->setEventLoopLocker(eventLoopLocker);

        m_data->m_taskExecutor->add(std::unique_ptr<IdxDataDeepFetcher>(fetcher));
        eventLoop.exec();
    }
}


bool IdxDataManager::canFetchMore(const QModelIndex& _parent)
{
    IdxData* idxData = getIdxDataFor(_parent);
    const bool status = idxData->status() == NodeStatus::NotFetched;

    return status;
}


void IdxDataManager::setDatabase(Database::IDatabase* database)
{
    if (m_data->m_database != nullptr)
        disconnect(m_data->m_database->notifier());

    m_data->m_database = database;

    if (database != nullptr)
    {
        connect(m_data->m_database->notifier(), &Database::ADatabaseSignals::photoModified, this, &IdxDataManager::photoChanged);
        connect(m_data->m_database->notifier(), &Database::ADatabaseSignals::photosAdded,   this, &IdxDataManager::photosAdded);
        connect(m_data->m_database->notifier(), &Database::ADatabaseSignals::photosRemoved, this, &IdxDataManager::photosRemoved);
    }

    resetModel();
}


void IdxDataManager::applyFilters(const SearchExpressionEvaluator::Expression& filters)
{
    m_data->filterExpression = filters;

    IdxData* root = getRoot();
    refetchNode(root);
}


void IdxDataManager::refetchNode(IdxData* _parent)
{
    const NodeStatus current = _parent->status();

    if (current != NodeStatus::NotFetched)
    {
        removeChildren(_parent);
        _parent->reset();

        const QModelIndex idx = getIndex(_parent);
        fetchData(idx);
    }
}


IdxData* IdxDataManager::getRoot()
{
    return m_data->m_root;
}


IdxData* IdxDataManager::getIdxDataFor(const QModelIndex& obj) const
{
    IdxData* idxData = obj.isValid()?
                            static_cast<IdxData *>(obj.internalPointer()):
                            m_data->m_root;

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
    IdxData* idxData = getIdxDataFor(_parent);

    if (m_data->m_database)
    {
        if (idxData->status() != NodeStatus::Fetched)
            status = true;              //data not loaded assume there is something
        else
            status = idxData->isNode(); //return true for nodes only, not for leafs
    }

    return status;
}


IdxData* IdxDataManager::parent(const QModelIndex& child)
{
    IdxData* idxData = getIdxDataFor(child);
    IdxData* result  = idxData->parent();

    return result;
}


void IdxDataManager::getPhotosFor(const IdxData* idx, std::vector<IPhotoInfo::Ptr>* result)
{
    forIndexChildren(idx, [&] (const IdxData* child)
    {
        if (child->status() == NodeStatus::Fetched)
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


//function returns list of tag values on particular 'level' for 'parent'
void IdxDataManager::fetchTagValuesFor(size_t level, const QModelIndex& _parent)
{
    if (level <= m_data->m_hierarchy.nodeLevels())
    {
        std::deque<Database::IFilter::Ptr> filter;

        const TagNameInfo& tagNameInfo = m_data->m_hierarchy.getNodeInfo(level).tagName;
        buildFilterFor(_parent, &filter);
        buildExtraFilters(&filter);

        using namespace std::placeholders;
        auto callback = std::bind(&IdxDataManager::gotTagValuesForParent, this, _1, _2);
        auto safe_callback =
            m_data->m_tasksResultsCtrl.make_safe_callback< void(Database::AListTagValuesTask *, const std::deque<TagValue> &) >(callback);

        std::unique_ptr<Database::AListTagValuesTask> task = std::make_unique<ListTagValuesTask>(safe_callback, _parent, level);
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
    using namespace std::placeholders;
    auto callback = std::bind(&IdxDataManager::gotPhotosForParent, this, _1, _2);
    auto safe_callback =
        m_data->m_tasksResultsCtrl.make_safe_callback< void(Database::AGetPhotosTask *, const IPhotoInfo::List &) >(callback);

    std::unique_ptr<Database::AGetPhotosTask> task(new GetPhotosTask(safe_callback, _parent));

    //send task to execution
    m_data->m_database->exec(std::move(task), filter);
}


// function checks if there are photos which do not have tags required by particular parent in data model
void IdxDataManager::checkForNonmatchingPhotos(size_t level, const QModelIndex& _parent)
{
    std::deque<Database::IFilter::Ptr> filter;

    //build filters for all parent nodes but last one
    if (_parent.isValid())   // do not enter here if there are no parent above '_parent'
    {
        QModelIndex grandParent = _parent.parent();

        buildFilterFor(grandParent, &filter);
    }

    //add anti-filter for last node
    auto node_filter = std::make_shared<Database::FilterNotMatchingFilter>();
    auto tag_filter = std::make_shared<Database::FilterPhotosWithTag>(m_data->m_hierarchy.getNodeInfo(level).tagName);

    node_filter->filter = tag_filter;
    filter.push_back(node_filter);

    //model related filters
    buildExtraFilters(&filter);

    //prepare task and store it in local list
    using namespace std::placeholders;
    auto callback = std::bind(&IdxDataManager::gotNonmatchingPhotosForParent, this, _1, _2);
    auto safe_callback =
        m_data->m_tasksResultsCtrl.make_safe_callback< void( Database::AGetPhotosCount * , int ) >(callback);

    std::unique_ptr<Database::AGetPhotosCount> task(new GetNonmatchingPhotosTask(safe_callback, _parent));

    //send task to execution
    m_data->m_database->exec(std::move(task), filter);
}


void IdxDataManager::buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter)
{
    IdxData* idxData = getIdxDataFor(_parent);

    filter->push_back(idxData->m_filter);

    // append parent's filters
    if (idxData->m_level > 0)
        buildFilterFor(_parent.parent(), filter);
}


void IdxDataManager::buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const
{
    const auto modelSpecificFilters = m_data->m_model->getStaticFilters();
    filter->insert(filter->end(), modelSpecificFilters.begin(), modelSpecificFilters.end());

    if (m_data->filterExpression.empty() == false)
    {
        const auto searchExpressionFilter = std::make_shared<Database::FilterPhotosMatchingExpression>(m_data->filterExpression);
        filter->push_back(searchExpressionFilter);
    }
}


void IdxDataManager::fetchData(const QModelIndex& _parent)
{
    IdxData* idxData = getIdxDataFor(_parent);
    const size_t level = idxData->m_level;

    assert(idxData->status() == NodeStatus::NotFetched);
    assert(level <= m_data->m_hierarchy.nodeLevels());

    const bool leaves_level = level == m_data->m_hierarchy.nodeLevels();   //leaves level is last level of hierarchy

    if (leaves_level)                  //construct leaves basing on photos
        fetchPhotosFor(_parent);
    else                               //construct nodes basing on tags
    {
        fetchTagValuesFor(level, _parent);
        checkForNonmatchingPhotos(level, _parent);
    }

    markIdxDataBeingFetched(idxData);
}


void IdxDataManager::setupNewNode(IdxData* node, const Database::IFilter::Ptr& filter, const Hierarchy::Level& order) const
{
    assert(node->isNode());

    node->setNodeFilter(filter);
    node->setNodeSorting(order);
}


void IdxDataManager::setupRootNode()
{
    const auto& topLevel = m_data->m_hierarchy.getNodeInfo(0);

    getRoot()->setNodeSorting(topLevel);
}


//called when leafs for particular node have been loaded
void IdxDataManager::gotPhotosForParent(Database::AGetPhotosTask* task, const IPhotoInfo::List& photos)
{
    GetPhotosTask* l_task = static_cast<GetPhotosTask *>(task);
    IdxData* parentIdxData = getIdxDataFor(l_task->m_parent);

    std::shared_ptr<std::deque<IdxData *>> leafs = std::make_shared<std::deque<IdxData *>>();

    // search for groups
    std::map<Group::Id, std::deque<IPhotoInfo::Ptr>> grouped;
    IPhotoInfo::List ungrouped;

    for(IPhotoInfo::Ptr photoInfo: photos)
    {
        Group::Id id = photoInfo->data().group_id;

        if (id.valid())
            grouped[id].push_back(photoInfo);
        else
            ungrouped.push_back(photoInfo);
    }

    // create leafs for ungrouped photos
    for(IPhotoInfo::Ptr photoInfo: ungrouped)
    {
        IdxData* newItem = new IdxData(this, photoInfo );
        leafs->push_back(newItem);
    }

    //attach nodes to parent node in main thread
    emit nodesFetched(parentIdxData, leafs);
}


//called when we look for photos which do not have tag required by particular parent
void IdxDataManager::gotNonmatchingPhotosForParent(Database::AGetPhotosCount* task, int size)
{
    if (size > 0)  //there is at least one such a photo? Create extra node
    {
        std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

        GetNonmatchingPhotosTask* l_task = static_cast<GetNonmatchingPhotosTask *>(task);
        QModelIndex _parentIndex = l_task->m_parent;
        IdxData* _parent = getIdxDataFor(_parentIndex);
        IdxData* node = prepareUniversalNodeFor(_parent);

        leafs->push_back(node);

        emit nodesFetched(_parent, leafs);
    }
}


//called when nodes for particual node have been loaded
void IdxDataManager::gotTagValuesForParent(Database::AListTagValuesTask* task, const std::deque<TagValue>& tags)
{
    ListTagValuesTask* l_task = static_cast<ListTagValuesTask *>(task);

    const size_t level = l_task->m_level;
    const QModelIndex& _parent = l_task->m_parent;
    IdxData* parentIdxData = getIdxDataFor(_parent);

    std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

    for(const TagValue& tag: tags)
    {
        auto filter = std::make_shared<Database::FilterPhotosWithTag>(m_data->m_hierarchy.getNodeInfo(level).tagName, tag);

        IdxData* newItem = new IdxData(m_data->m_root->m_model, tag.get());
        setupNewNode(newItem, filter, m_data->m_hierarchy.getNodeInfo(level + 1));

        leafs->push_back(newItem);
    }

    //attach nodes to parent node in main thread
    emit nodesFetched(parentIdxData, leafs);
}


void IdxDataManager::markIdxDataFetched(IdxData* idxData)
{
    idxData->setStatus(NodeStatus::Fetched);
    removeIdxDataFromNotFetched(idxData);
    emit dataChanged(idxData, { DBDataModel::NodeStatus } );
}


void IdxDataManager::markIdxDataBeingFetched(IdxData* idxData)
{
    idxData->setStatus(NodeStatus::Fetching);
    emit dataChanged(idxData, { DBDataModel::NodeStatus } );
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

    // drop any result from currently pending tasks
    m_data->m_tasksResultsCtrl.invalidate();

    m_data->m_model->beginResetModel();
    m_data->m_root->reset();
    setupRootNode();
    m_data->m_model->endResetModel();
}


void IdxDataManager::appendIdxData(IdxData* _parent, const std::deque<IdxData *>& nodes)
{
    assert(nodes.empty() == false);
    // We are not expecting any sub-nodes for not fetched nodes.
    // Only fully fetched and being fetched nodes may accept something.
    assert(_parent->status() != NodeStatus::NotFetched);

    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    const QModelIndex parentIdx = m_data->m_model->createIndex(_parent);
    assert(parentIdx.isValid() || _parent->parent() == nullptr);               // parentIdx may be invalid only in one case - then _parent is top

    //This function should be used only when writing to an empty parent.
    //Reason for it is that addChild does sorting and items may go
    //to other positions than reported by beginInsertRows() and endInsertRows()
    if(_parent->m_children.empty())
    {
        const int last = static_cast<int>(nodes.size()) - 1;
        assert(last >= 0);

        m_data->m_model->beginInsertRows(parentIdx, 0, last);

        for(IdxData* newItem: nodes)
            _parent->addChild(newItem);

        m_data->m_model->endInsertRows();
    }
    else                                               //model already has something. Add items one by one
        for(IdxData* idx: nodes)
            performAdd(_parent, idx);
}


bool IdxDataManager::movePhotoToRightParent(const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxData* currentParent = getCurrentParent(photoInfo);
    IdxData* newParent = createAncestry(photoInfo);
    bool position_changed = currentParent != newParent;

    if (position_changed)
    {
        if (currentParent == nullptr)
            performAdd(photoInfo, newParent);
        else if (newParent == nullptr)
            performRemove(photoInfo);
        else
            performMove(photoInfo, currentParent, newParent);
    }
    else if (currentParent != nullptr)                            //same parent, but maybe reorder of children is required? (sorting)
        position_changed = sortChildrenOf(currentParent);

    return position_changed;
}


IdxData* IdxDataManager::getCurrentParent(const IPhotoInfo::Ptr& photoInfo)
{
    IdxData* item = findIdxDataFor(photoInfo);
    IdxData* result = item != nullptr? item->parent(): nullptr;

    return result;
}


IdxData* IdxDataManager::createAncestry(const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

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
            if (_parent->status() != NodeStatus::Fetched)
            {
                //Ancestor of photo isn't yet fetched. Don't fetch it. We will do it on user's demand
                //Just make sure we will return _parent == nullptr as we didn't achieve direct parent
                _parent = nullptr;
            }
        }
        else   // we could not find or create any reasonable parent. Create or attach to universal parent for such a orphans (if possible)
            createUniversalAncestor(&matcher, photoInfo);
    }

    return _parent;
}


IdxData* IdxDataManager::findIdxDataFor(const IPhotoInfo::Ptr& photoInfo)
{
    const Photo::Id id = photoInfo->getID();
    IdxData* result = findIdxDataFor(id);

    return result;
}


IdxData* IdxDataManager::findIdxDataFor(const Photo::Id& id)
{
    auto photosMap = m_data->m_photoId2IdxData.lock();
    auto it = photosMap->find(id);
    IdxData* result = nullptr;

    if (it != photosMap->end())
        result = it->second;

    return result;
}


IdxData *IdxDataManager::createCloserAncestor(PhotosMatcher* matcher, const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxData* _parent = matcher->findCloserAncestorFor(photoInfo);
    IdxData* result = nullptr;

    const Tag::TagsList& photoTags = photoInfo->getTags();
    const size_t level = _parent->m_level;

    // found parent is not fetched? Quit with null
    if (_parent->status() == NodeStatus::Fetched)
    {
        if (level == m_data->m_hierarchy.nodeLevels())  //this parent is at the bottom of hierarchy? Just use it as result
            result = _parent;
        else
        {
            const TagNameInfo& tagName = m_data->m_hierarchy.getNodeInfo(level).tagName;
            auto photoTagIt = photoTags.find(tagName);

            //we need to add subnode for '_parent' we are sure it doesn't exist as 'createRightParent' takes closer ancestor for '_parent'
            if (photoTagIt != photoTags.end())
            {
                const TagValue& tagValue = photoTagIt->second;
                IdxData* node = new IdxData(this, tagValue.get());

                auto filter = std::make_shared<Database::FilterPhotosWithTag>(tagName, tagValue);

                setupNewNode(node, filter, m_data->m_hierarchy.getNodeInfo(level + 1));
                performAdd(_parent, node);

                result = node;
            }
            else
            {
                //photo doesn't match. It may be not loaded yet or has incomplete tags list
            }
        }
    }

    return result;
}


IdxData* IdxDataManager::createUniversalAncestor(PhotosMatcher* matcher, const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxData* _parent = matcher->findCloserAncestorFor(photoInfo);
    IdxData* universalNode = nullptr;

    //closer parent is not fetched? Do no create anything for it
    if (_parent->status() == NodeStatus::Fetched)
    {
        universalNode = prepareUniversalNodeFor(_parent);

        performAdd(_parent, universalNode);
    }

    return universalNode;
}


void IdxDataManager::removeChildren(IdxData* parent)
{
    performRemoveChildren(parent);
}


void IdxDataManager::performMove(const IPhotoInfo::Ptr& photoInfo, IdxData* from, IdxData* to)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxData* photoIdxData = findIdxDataFor(photoInfo);
    performMove(photoIdxData, from, to);
}


void IdxDataManager::performMove(IdxData* item, IdxData* from, IdxData* to)
{
    QModelIndex fromIdx = getIndex(from);
    QModelIndex toIdx = getIndex(to);
    const int fromPos = item->getRow();
    const int toPos = static_cast<int>( to->findPositionFor(item) );

    m_data->m_model->beginMoveRows(fromIdx, fromPos, fromPos, toIdx, toPos);

    from->takeChild(item);
    to->addChild(item);

    m_data->m_model->endMoveRows();

    //remove empty parents
    if (from->m_children.empty())
        performRemove(from);
}

void IdxDataManager::performRemoveChildren(IdxData* parent)
{
    // modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    // only root item can be empty
    assert(parent == m_data->m_root || parent->m_children.empty() == false);

    QModelIndex parentIdx = getIndex(parent);

    // do not work on children directly as we will drop them
    std::vector<IdxData *> children = parent->m_children;

    if (children.empty() == false)
    {
        m_data->m_model->beginRemoveRows(parentIdx, 0, static_cast<int>( children.size() ) - 1);

        for(IdxData* c: children)
            parent->removeChild(c);

        m_data->m_model->endRemoveRows();

        //remove empty parent (exclude root item)
        if (parent != m_data->m_root)
            performRemove(parent);
    }
}


void IdxDataManager::performRemove(const IPhotoInfo::Ptr& photoInfo)
{
    const Photo::Id& id = photoInfo->getID();
    performRemove(id);
}


void IdxDataManager::performRemove(const Photo::Id& id)
{
    IdxData* photoIdxData = findIdxDataFor(id);

    if (photoIdxData)
        performRemove(photoIdxData);
}


void IdxDataManager::performRemove(IdxData* item)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    if (item != m_data->m_root)   // never drop root
    {
        IdxData* _parent = item->parent();
        assert(_parent != nullptr);

        QModelIndex parentIdx = getIndex(_parent);
        const int itemPos = item->getRow();

        m_data->m_model->beginRemoveRows(parentIdx, itemPos, itemPos);

        _parent->removeChild(item);

        m_data->m_model->endRemoveRows();

        //remove empty parents
        if (_parent->m_children.empty())
            performRemove(_parent);
    }
}


void IdxDataManager::performAdd(const IPhotoInfo::Ptr& photoInfo, IdxData* to)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxData* photoIdxData = findIdxDataFor(photoInfo);

    if (photoIdxData == nullptr)
        photoIdxData = new IdxData(this, photoInfo);

    performAdd(to, photoIdxData);
}


void IdxDataManager::performAdd(IdxData* _parent, IdxData* item)
{
    QModelIndex toIdx = getIndex(_parent);
    const int toPos = static_cast<int>( _parent->findPositionFor(item) );

    m_data->m_model->beginInsertRows(toIdx, toPos, toPos);

    _parent->addChild(item);

    m_data->m_model->endInsertRows();
}


bool IdxDataManager::sortChildrenOf(IdxData* _parent)
{
    const bool result = _parent->sortingRequired();
    bool dirty = result;

    while (dirty)
    {
        IdxData* child = _parent->findChildWithBadPosition();

        if (child == nullptr)
            dirty = false;
        else
            performMove(child, _parent, _parent);
    }

    return result;
}


IdxData* IdxDataManager::prepareUniversalNodeFor(IdxData* _parent)
{
    IdxData* node = new IdxData(this, tr("Unlabeled"));

    const size_t level = _parent->m_level;
    const TagNameInfo& tagName = m_data->m_hierarchy.getNodeInfo(level).tagName;

    auto filterTag = std::make_shared<Database::FilterPhotosWithTag>(tagName);
    auto filter = std::make_shared<Database::FilterNotMatchingFilter>();
    filter->filter = filterTag;

    setupNewNode(node, filter, m_data->m_hierarchy.getNodeInfo(level + 1) );

    return node;
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

        //take IdxData now, it is possible we have removed it while moving to new parent
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


void IdxDataManager::photosAdded(const std::deque<IPhotoInfo::Ptr>& photoInfos)
{
    PhotosMatcher matcher;
    matcher.set(this);
    matcher.set(m_data->m_model);

    for(const IPhotoInfo::Ptr& ptr: photoInfos)
    {
        const bool match = matcher.doesMatchModelFilters(ptr);

        if (match)
            movePhotoToRightParent(ptr);
    }
}


void IdxDataManager::photosRemoved(const std::deque<Photo::Id>& photos)
{
    for(const Photo::Id& id: photos)
        performRemove(id);
}
