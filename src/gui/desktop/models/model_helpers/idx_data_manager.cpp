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
#include <core/function_wrappers.hpp>
#include <database/iphoto_info.hpp>
#include <database/database_tools/signal_mapper.hpp>

#include "idxdata_deepfetcher.hpp"
#include "photos_matcher.hpp"


Q_DECLARE_METATYPE(std::shared_ptr<std::vector<IdxData *>>)
Q_DECLARE_METATYPE(std::vector<IPhotoInfo::Ptr>)


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

        m_root.reset();
    }

    void init(IdxDataManager* manager)
    {
        assert(m_root == nullptr);
        m_root = std::make_unique<IdxNodeData>(manager, "");
        m_root->setParent(nullptr);
    }

    Data(const Data &) = delete;
    Data& operator=(const Data &) = delete;

    DBDataModel* m_model;
    std::unique_ptr<IdxNodeData> m_root;
    Hierarchy m_hierarchy;
    Database::IDatabase* m_database;
    ol::ThreadSafeResource<std::unordered_map<Photo::Id, IIdxData *, Photo::IdHash>> m_photoId2IdxData;
    ol::ThreadSafeResource<std::unordered_set<IIdxData *>> m_notFetchedIdxData;
    std::thread::id m_mainThreadId;
    ITaskExecutor* m_taskExecutor;
    safe_callback_ctrl m_tasksResultsCtrl;
    SearchExpressionEvaluator::Expression filterExpression;
    Database::SignalMapper m_signalsMapper;
};


IdxDataManager::INotifications::~INotifications()
{

}


IdxDataManager::IdxDataManager(DBDataModel* model): m_data(new Data(model))
{
    m_data->init(this);

    //default hierarchy
    const Hierarchy hierarchy = {
                                  { TagTypeInfo(TagTypes::Date), Hierarchy::Level::Order::ascending },
                                  { TagTypeInfo(TagTypes::Time), Hierarchy::Level::Order::ascending }
                                };

    setHierarchy(hierarchy);

    qRegisterMetaType< std::shared_ptr<std::vector<IdxData *>> >();
    qRegisterMetaType<IPhotoInfo::Ptr>("IPhotoInfo::Ptr");
    qRegisterMetaType<std::vector<IPhotoInfo::Ptr>>();
    qRegisterMetaType<Photo::Id>();
    qRegisterMetaType<std::vector<Photo::Id>>();

    connect(&m_data->m_signalsMapper, &Database::SignalMapper::photoModified, this, &IdxDataManager::photoChanged);
    connect(&m_data->m_signalsMapper, &Database::SignalMapper::photosAdded,   this, &IdxDataManager::photosAdded);
    connect(&m_data->m_signalsMapper, &Database::SignalMapper::photosRemoved, this, &IdxDataManager::photosRemoved);
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


void IdxDataManager::deepFetch(IIdxData* top)
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
    IIdxData* idxData = getIdxDataFor(_parent);
    const bool status = idxData->status() == NodeStatus::NotFetched;

    return status;
}


void IdxDataManager::setDatabase(Database::IDatabase* database)
{
    m_data->m_database = database;
    m_data->m_signalsMapper.set(database);

    resetModel();
}


void IdxDataManager::applyFilters(const SearchExpressionEvaluator::Expression& filters)
{
    m_data->filterExpression = filters;

    IdxNodeData* root = getRoot();
    refetchNode(root);
}


void IdxDataManager::refetchNode(IdxNodeData* _parent)
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


IdxNodeData* IdxDataManager::getRoot()
{
    return m_data->m_root.get();
}


IIdxData* IdxDataManager::getIdxDataFor(const QModelIndex& obj) const
{
    IIdxData* idxData = obj.isValid()?
                            static_cast<IIdxData *>(obj.internalPointer()):
                            m_data->m_root.get();

    return idxData;
}


QModelIndex IdxDataManager::getIndex(IIdxData* idxData) const
{
    return m_data->m_model->createIndex(idxData);
}


bool IdxDataManager::hasChildren(const QModelIndex& _parent)
{
    // Always return true for nonloaded nodes.
    // This prevents view from calling rowCount() before canFetchMore()

    bool status = false;
    IIdxData* idxData = getIdxDataFor(_parent);

    apply_inline_visitor(idxData,
                         [&status] (const IdxNodeData *)        { status = true;  },
                         [&status] (const IdxRegularLeafData *) { status = false; },
                         [&status] (const IdxGroupLeafData *)   { status = false; }
    );

    return status;
}


IdxNodeData* IdxDataManager::parent(const QModelIndex& child)
{
    IIdxData* idxData = getIdxDataFor(child);
    IdxNodeData* result  = idxData->parent();

    return result;
}


void IdxDataManager::idxDataCreated(IIdxData* idxData)
{
    addIdxDataToNotFetched(idxData);

    if (isLeaf(idxData))
    {
        IdxLeafData* leafData = static_cast<IdxLeafData *>(idxData);
        m_data->m_photoId2IdxData.lock()->insert( std::make_pair(leafData->getMediaId(), idxData) );
    }
}


void IdxDataManager::idxDataDeleted(IdxLeafData* idxData)
{
    removeIdxDataFromNotFetched(idxData);

    m_data->m_photoId2IdxData.lock()->erase(idxData->getMediaId());
}


void IdxDataManager::idxDataDeleted(IdxNodeData* idxData)
{
    removeIdxDataFromNotFetched(idxData);
}


void IdxDataManager::idxDataReset(IIdxData* idxData)
{
    addIdxDataToNotFetched(idxData);
}


//function returns list of tag values on particular 'level' for 'parent'
void IdxDataManager::fetchTagValuesFor(size_t level, const QModelIndex& _parent)
{
    if (level <= m_data->m_hierarchy.nodeLevels())
    {
        std::vector<Database::IFilter::Ptr> filter;

        const TagTypeInfo& tagNameInfo = m_data->m_hierarchy.getNodeInfo(level).tagName;
        buildFilterFor(_parent, &filter);
        buildExtraFilters(&filter);

        using namespace std::placeholders;
        auto callback = std::bind(&IdxDataManager::gotTagValuesForParent, this, _parent, level, _2);
        auto safe_callback =
            m_data->m_tasksResultsCtrl.make_safe_callback< void(const TagTypeInfo &, const std::vector<TagValue> &) >(callback);

        m_data->m_database->listTagValues(tagNameInfo, filter, safe_callback);
    }
    else
        assert(!"should not happend");
}


void IdxDataManager::fetchPhotosFor(const QModelIndex& _parent)
{
    std::vector<Database::IFilter::Ptr> filter;
    buildFilterFor(_parent, &filter);
    buildExtraFilters(&filter);

    using namespace std::placeholders;
    auto action_callback = std::bind(&IdxDataManager::getPhotosForParent, this, _1, _parent, filter);

    m_data->m_database->exec(action_callback);
}


// function checks if there are photos which do not have tags required by particular parent in data model
void IdxDataManager::checkForNonmatchingPhotos(size_t level, const QModelIndex& _parent)
{
    std::vector<Database::IFilter::Ptr> filter;

    //build filters for all parent nodes but last one
    if (_parent.isValid())   // do not enter here if there are no parent above '_parent'
    {
        QModelIndex grandParent = _parent.parent();

        buildFilterFor(grandParent, &filter);
    }

    //add anti-filter for last node
    auto tag_filter = std::make_shared<Database::FilterPhotosWithTag>(m_data->m_hierarchy.getNodeInfo(level).tagName);
    auto node_filter = std::make_shared<Database::FilterNotMatchingFilter>(tag_filter);

    filter.push_back(node_filter);

    //model related filters
    buildExtraFilters(&filter);

    //prepare task and store it in local list
    using namespace std::placeholders;
    auto callback = std::bind(&IdxDataManager::gotNonmatchingPhotosForParent, this, _parent, _1);
    auto safe_callback =
        m_data->m_tasksResultsCtrl.make_safe_callback<void(int)>(callback);

    //send task to execution
    m_data->m_database->countPhotos(filter, safe_callback);
}


void IdxDataManager::buildFilterFor(const QModelIndex& _parent, std::vector<Database::IFilter::Ptr>* filter)
{
    IIdxData* idxData = getIdxDataFor(_parent);

    filter->push_back(idxData->getFilter());

    // append parent's filters
    if (idxData->getLevel() > 0)
        buildFilterFor(_parent.parent(), filter);
}


void IdxDataManager::buildExtraFilters(std::vector<Database::IFilter::Ptr>* filter) const
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
    IIdxData* idxData = getIdxDataFor(_parent);
    const size_t level = idxData->getLevel();

    assert(idxData->status() == NodeStatus::NotFetched);
    assert(level <= m_data->m_hierarchy.nodeLevels());

    const bool leaves_level = level == m_data->m_hierarchy.nodeLevels();   //leaves level is last level of hierarchy

    // mark this node as being fetched, as below we start fetching data
    markIdxDataBeingFetched(idxData);

    if (leaves_level)                  //construct leaves basing on photos
        fetchPhotosFor(_parent);
    else                               //construct nodes basing on tags
    {
        fetchTagValuesFor(level, _parent);
        checkForNonmatchingPhotos(level, _parent);
    }
}


void IdxDataManager::setupNewNode(IIdxData* node, const Database::IFilter::Ptr& filter, const Hierarchy::Level& order) const
{
    assert(isNode(node));

    node->setNodeFilter(filter);
    node->setNodeSorting(order);
}


void IdxDataManager::setupRootNode()
{
    const auto& topLevel = m_data->m_hierarchy.getNodeInfo(0);

    getRoot()->setNodeSorting(topLevel);
}


void IdxDataManager::getPhotosForParent(Database::IBackend* db_operator, const QModelIndex& parent, const std::vector<Database::IFilter::Ptr>& filter)
{
    auto photos = db_operator->getPhotos(filter);
    auto leafs = std::make_shared<std::vector<IIdxData::Ptr>>();

    Group::Id current_group;

    for(const Photo::Id& id: photos)
    {
        Database::IUtils* utils = m_data->m_database->utils();

        IPhotoInfo::Ptr photo = utils->getPhotoFor(id);

        const Photo::Data pData = photo->data();
        const Group::Id gid = pData.groupInfo.group_id;

        if (gid.valid())
        {
            if (gid != current_group)
            {
                assert(pData.groupInfo.role == GroupInfo::Representative);       // we do not expect members here
                leafs->push_back( std::make_unique<IdxGroupLeafData>(this, pData) );
            }
        }
        else
            leafs->push_back( std::make_unique<IdxRegularLeafData>(this, pData) );

        current_group = gid;
    }

    //attach nodes to parent node in main thread
    IIdxData* parentIdxData = getIdxDataFor(parent);

    assert(isNode(parentIdxData));
    IdxNodeData* parentNode = static_cast<IdxNodeData *>(parentIdxData);

    call_from_this_thread(this, std::bind(&IdxDataManager::insertFetchedNodes, this, parentNode, leafs));  // call insertFetchedNodes from thread owning 'this'
}


//called when we look for photos which do not have tag required by particular parent
void IdxDataManager::gotNonmatchingPhotosForParent(const QModelIndex& parent, int size)
{
    if (size > 0)  //there is at least one such a photo? Create extra node
    {
        auto leafs = std::make_shared<std::vector<IdxData::Ptr>>();

        IIdxData* _parent = getIdxDataFor(parent);
        IIdxData::Ptr node = prepareUniversalNodeFor(_parent);

        assert(isNode(_parent));
        IdxNodeData* parent_node = static_cast<IdxNodeData *>(_parent);

        leafs->push_back(std::move(node));

        call_from_this_thread(this, std::bind(&IdxDataManager::insertFetchedNodes, this, parent_node, std::move(leafs)));
    }
}


//called when nodes for particual node have been loaded
void IdxDataManager::gotTagValuesForParent(const QModelIndex& parent, std::size_t level, const std::vector<TagValue>& tags)
{
    IIdxData* parentIdxData = getIdxDataFor(parent);

    assert(isNode(parentIdxData));
    IdxNodeData* parentNode = static_cast<IdxNodeData *>(parentIdxData);

    auto leafs = std::make_shared<std::vector<IIdxData::Ptr>>();

    for(const TagValue& tag: tags)
    {
        auto filter = std::make_shared<Database::FilterPhotosWithTag>(m_data->m_hierarchy.getNodeInfo(level).tagName, tag);

        auto newItem = std::make_unique<IdxNodeData>(this, tag.get());
        setupNewNode(newItem.get(), filter, m_data->m_hierarchy.getNodeInfo(level + 1));

        leafs->push_back(std::move(newItem));
    }

    //attach nodes to parent node in main thread
    call_from_this_thread(this, std::bind(&IdxDataManager::insertFetchedNodes, this, parentNode, std::move(leafs)));
}


void IdxDataManager::markIdxDataFetched(IIdxData* idxData)
{
    idxData->setStatus(NodeStatus::Fetched);
    removeIdxDataFromNotFetched(idxData);
    emit dataChanged(idxData, { DBDataModel::NodeStatus } );
}


void IdxDataManager::markIdxDataBeingFetched(IIdxData* idxData)
{
    idxData->setStatus(NodeStatus::Fetching);
    emit dataChanged(idxData, { DBDataModel::NodeStatus } );
}


void IdxDataManager::removeIdxDataFromNotFetched(IIdxData* idxData)
{
    m_data->m_notFetchedIdxData.lock()->erase(idxData);
}


void IdxDataManager::addIdxDataToNotFetched(IIdxData* idxData)
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


void IdxDataManager::appendIdxData(IdxNodeData* _parent, const std::shared_ptr<std::vector<IIdxData::Ptr>>& nodes)
{
    assert(nodes->empty() == false);
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
    if(_parent->getChildren().empty())
    {
        const int last = static_cast<int>(nodes->size()) - 1;
        assert(last >= 0);

        m_data->m_model->beginInsertRows(parentIdx, 0, last);

        for(IIdxData::Ptr& newItem: *nodes)
            _parent->addChild(std::move(newItem));

        m_data->m_model->endInsertRows();
    }
    else                                               //model already has something. Add items one by one
        for(IIdxData::Ptr& idx: *nodes)
            performAdd(_parent, std::move(idx));
}


bool IdxDataManager::movePhotoToRightParent(const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxNodeData* currentParent = getCurrentParent(photoInfo->getID());
    IdxNodeData* newParent = createAncestry(photoInfo);
    bool position_changed = currentParent != newParent;

    if (position_changed)
    {
        if (currentParent == nullptr)
            performAdd(photoInfo, newParent);
        else if (newParent == nullptr)
            performRemove(photoInfo->getID());
        else
            performMove(photoInfo->getID(), currentParent, newParent);
    }
    else if (currentParent != nullptr)                            //same parent, but maybe reorder of children is required? (sorting)
        position_changed = sortChildrenOf(currentParent);

    return position_changed;
}


IdxNodeData* IdxDataManager::getCurrentParent(const Photo::Id& id)
{
    IIdxData* item = findIdxDataFor(id);
    IdxNodeData* result = item != nullptr? item->parent(): nullptr;

    return result;
}


IdxNodeData* IdxDataManager::createAncestry(const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    PhotosMatcher matcher;
    matcher.set(this);
    matcher.set(m_data->m_model);
    IdxNodeData* _parent = nullptr;

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
        else   // we could not find or create any reasonable parent. Create or attach to universal parent for such orphans (if possible)
            createUniversalAncestor(&matcher, photoInfo);
    }

    return _parent;
}


IIdxData* IdxDataManager::findIdxDataFor(const Photo::Id& id)
{
    auto photosMap = m_data->m_photoId2IdxData.lock();
    auto it = photosMap->find(id);
    IIdxData* result = nullptr;

    if (it != photosMap->end())
        result = it->second;

    return result;
}


IdxNodeData* IdxDataManager::createCloserAncestor(PhotosMatcher* matcher, const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxNodeData* _parent = matcher->findCloserAncestorFor(photoInfo);
    IdxNodeData* result = nullptr;

    const Tag::TagsList& photoTags = photoInfo->getTags();
    const size_t level = _parent->getLevel();

    // found parent is not fetched? Quit with null
    if (_parent->status() == NodeStatus::Fetched)
    {
        if (level == m_data->m_hierarchy.nodeLevels())  //this parent is at the bottom of hierarchy? Just use it as result
            result = _parent;
        else
        {
            const TagTypeInfo& tagName = m_data->m_hierarchy.getNodeInfo(level).tagName;
            auto photoTagIt = photoTags.find(tagName);

            //we need to add subnode for '_parent' we are sure it doesn't exist as 'createRightParent' takes closer ancestor for '_parent'
            if (photoTagIt != photoTags.end())
            {
                const TagValue& tagValue = photoTagIt->second;
                auto node = std::make_unique<IdxNodeData>(this, tagValue.get());
                auto filter = std::make_shared<Database::FilterPhotosWithTag>(tagName, tagValue);

                setupNewNode(node.get(), filter, m_data->m_hierarchy.getNodeInfo(level + 1));
                IIdxData* added_node = performAdd(_parent, std::move(node));

                isNode(added_node);

                result = static_cast<IdxNodeData *>(added_node);
            }
            else
            {
                //photo doesn't match. It may be not loaded yet or has incomplete tags list
            }
        }
    }

    return result;
}


IdxNodeData* IdxDataManager::createUniversalAncestor(PhotosMatcher* matcher, const IPhotoInfo::Ptr& photoInfo)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IdxNodeData* _parent = matcher->findCloserAncestorFor(photoInfo);
    IdxNodeData* result = nullptr;

    //closer parent is not fetched? Do no create anything for it
    if (_parent->status() == NodeStatus::Fetched)
    {
        IIdxData::Ptr universalNode = prepareUniversalNodeFor(_parent);

        IIdxData* added_node = performAdd(_parent, std::move(universalNode));

        result = static_cast<IdxNodeData *>(added_node);
    }

    return result;
}


void IdxDataManager::removeChildren(IdxNodeData* parent)
{
    performRemoveChildren(parent);
}


void IdxDataManager::performMove(const Photo::Id& id, IdxNodeData* from, IdxNodeData* to)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IIdxData* photoIdxData = findIdxDataFor(id);
    performMove(photoIdxData, from, to);
}


void IdxDataManager::performMove(IIdxData* item, IdxNodeData* from, IdxNodeData* to)
{
    QModelIndex fromIdx = getIndex(from);
    QModelIndex toIdx = getIndex(to);
    const int fromPos = item->getRow();
    const int toPos = static_cast<int>( to->findPositionFor(item) );

    m_data->m_model->beginMoveRows(fromIdx, fromPos, fromPos, toIdx, toPos);

    IIdxData::Ptr childPtr = from->takeChild(item);
    to->addChild( std::move(childPtr) );

    m_data->m_model->endMoveRows();

    //remove empty parents
    if (from->getChildren().empty())
        performRemove(from);
}

void IdxDataManager::performRemoveChildren(IdxNodeData* parent)
{
    // modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    // only root item can be empty
    assert(parent == m_data->m_root.get() || parent->getChildren().empty() == false);

    QModelIndex parentIdx = getIndex(parent);

    if (parent->getChildren().empty() == false)
    {
        m_data->m_model->beginRemoveRows(parentIdx, 0, static_cast<int>( parent->getChildren().size() ) - 1);

        parent->removeChildren();

        m_data->m_model->endRemoveRows();

        //remove empty parent (exclude root item)
        if (parent != m_data->m_root.get())
            performRemove(parent);
    }
}


void IdxDataManager::performRemove(const Photo::Id& id)
{
    IIdxData* photoIdxData = findIdxDataFor(id);

    if (photoIdxData)
        performRemove(photoIdxData);
}


void IdxDataManager::performRemove(IIdxData* item)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    if (item != m_data->m_root.get())   // never drop root
    {
        IdxNodeData* _parent = item->parent();

        assert(_parent != nullptr);

        QModelIndex parentIdx = getIndex(_parent);
        const int itemPos = item->getRow();

        m_data->m_model->beginRemoveRows(parentIdx, itemPos, itemPos);

        _parent->removeChild(item);

        m_data->m_model->endRemoveRows();

        //remove empty parents
        if (_parent->getChildren().empty())
            performRemove(_parent);
    }
}


IIdxData* IdxDataManager::performAdd(const IPhotoInfo::Ptr& photoInfo, IdxNodeData* to)
{
    //modify IdxData only in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    IIdxData* photoIdxData = findIdxDataFor(photoInfo->getID());

    // TODO: Code above looks weird. Watch for sitution when it returns non null ptr.
    //       See 2199bc8c8d5c10bf84ed7334ec9a6779be311639 for first change
    assert(photoIdxData == nullptr);

    auto newItem = std::make_unique<IdxRegularLeafData>(this, photoInfo->data());

    IIdxData* item = performAdd(to, std::move(newItem));

    return item;
}


IIdxData* IdxDataManager::performAdd(IdxNodeData* _parent, IIdxData::Ptr&& item)
{
    QModelIndex toIdx = getIndex(_parent);
    const int toPos = static_cast<int>( _parent->findPositionFor(item.get()) );

    m_data->m_model->beginInsertRows(toIdx, toPos, toPos);

    IIdxData* addedItem = _parent->addChild(std::move(item));

    m_data->m_model->endInsertRows();

    return addedItem;
}


bool IdxDataManager::sortChildrenOf(IdxNodeData* _parent)
{
    const bool result = _parent->sortingRequired();
    bool dirty = result;

    while (dirty)
    {
        IIdxData* child = _parent->findChildWithBadPosition();

        if (child == nullptr)
            dirty = false;
        else
            performMove(child, _parent, _parent);
    }

    return result;
}


IIdxData::Ptr IdxDataManager::prepareUniversalNodeFor(IIdxData* _parent)
{
    IdxData::Ptr node = std::make_unique<IdxNodeData>(this, tr("Unlabeled"));

    const size_t level = _parent->getLevel();
    const TagTypeInfo& tagName = m_data->m_hierarchy.getNodeInfo(level).tagName;

    auto filterTag = std::make_shared<Database::FilterPhotosWithTag>(tagName);
    auto filter = std::make_shared<Database::FilterNotMatchingFilter>(filterTag);

    setupNewNode(node.get(), filter, m_data->m_hierarchy.getNodeInfo(level + 1) );

    return node;
}


void IdxDataManager::insertFetchedNodes(IdxNodeData* _parent, const std::shared_ptr<std::vector<IIdxData::Ptr>>& nodes)
{
    //attach nodes to parent in main thread
    assert(m_data->m_mainThreadId == std::this_thread::get_id());

    if (nodes->empty() == false)
        appendIdxData(_parent, nodes);

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
        IIdxData* idx = findIdxDataFor(photoInfo->getID());
        if (idx != nullptr)
        {
            QModelIndex index = getIndex(idx);

            // update stored data
            assert(isLeaf(idx));
            IdxLeafData* idxLeaf = static_cast<IdxLeafData *>(idx);
            idxLeaf->update(photoInfo->data());

            // notify views
            emit m_data->m_model->dataChanged(index, index);
        }
    }
    else // photo doesn't match filters, but maybe it did?
    {
        IIdxData* idx = findIdxDataFor(photoInfo->getID());
        if (idx != nullptr)
            movePhotoToRightParent(photoInfo);
    }
}


void IdxDataManager::photosAdded(const std::vector<IPhotoInfo::Ptr>& photoInfos)
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


void IdxDataManager::photosRemoved(const std::vector<Photo::Id>& photos)
{
    for(const Photo::Id& id: photos)
        performRemove(id);
}
