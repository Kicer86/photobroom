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

#include <core/base_tags.hpp>
#include <database/query_list.hpp>
#include <database/filter.hpp>

#include <palgorithm/ts_resource.hpp>

#include "idx_data.hpp"

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

    struct DatabaseTaskHash
    {
        std::size_t operator()(const Database::Task& t) const
        {
            return t.getId();
        }
    };
}


struct DBDataModel::Impl: Database::IDatabaseClient
{
        Impl(DBDataModel* model):
            pThis(model),
            m_root(model, nullptr, "root"),
            m_hierarchy(),
            m_dirty(true),
            m_database(),
            m_iterator(),
            m_db_tasks()
        {
            m_root.setNodeData(std::make_shared<Database::FilterEmpty>()); //called just to mark root as node, not a leaf
            Hierarchy hierarchy;
            hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

            setHierarchy(hierarchy);
        }

        Impl(const Impl &) = delete;
        Impl& operator=(const Impl &) = delete;

        ~Impl() {}

        void setHierarchy(const Hierarchy& hierarchy)
        {
            m_hierarchy = hierarchy;
            m_dirty = true;
        }

        bool isDirty() const
        {
            return m_dirty;
        }

        void fetchMore(const QModelIndex& _parent)
        {
            IdxData* idxData = getParentIdxDataFor(_parent);
            const size_t level = idxData->m_level;

            if (level < m_hierarchy.levels.size())  //construct nodes basing on tags
                getLevelInfo(level, _parent);
            else
                if (level == m_hierarchy.levels.size())  //construct leafs basing on photos
                {
                    std::deque<Database::IFilter::Ptr> filter;
                    buildFilterFor(_parent, &filter);
                    buildExtraFilters(&filter);

                    //register task
                    Database::Task task = m_database->prepareTask(this);
                    m_db_tasks.lock().get()[task] = std::unique_ptr<ITaskData>(new GetPhotosTask(_parent));

                    //send task to execution
                    m_database->getPhotos(task, filter);
                }
                else
                    assert(!"should not happen");

            idxData->m_loaded = true;
        }

        bool canFetchMore(const QModelIndex& _parent)
        {
            IdxData* idxData = getParentIdxDataFor(_parent);
            const bool status = !idxData->m_loaded;

            return status;
        }

        void setBackend(Database::IDatabase* database)
        {
            m_database = database;
        }

        void close()
        {
            if (m_database)
                m_database->closeConnections();
        }

        IdxData* getIdxDataFor(const QModelIndex& obj) const
        {
            IdxData* idxData = static_cast<IdxData *>(obj.internalPointer());

            return idxData;
        }

        IdxData* getParentIdxDataFor(const QModelIndex& _parent)
        {
            IdxData* idxData = getIdxDataFor(_parent);

            if (idxData == nullptr)
                idxData = &m_root;

            return idxData;
        }

        bool hasChildren(const QModelIndex& _parent)
        {
            // Always return true for unloaded nodes.
            // This prevents view from calling rowCount() before canFetchMore()

            bool status = false;
            IdxData* idxData = getParentIdxDataFor(_parent);

            if (!idxData->m_loaded)
                status = true;              //data not loaded assume there is something
            else
                status = !idxData->m_photo; //return true for nodes only, not for leafs

            return status;
        }

        IdxData* parent(const QModelIndex& child)
        {
            IdxData* idxData = getIdxDataFor(child);
            IdxData* result  = idxData->m_parent;

            return result;
        }

        void addPhoto(const PhotoInfo::Ptr& photo)
        {
            m_root.addChild(photo);
        }

        void getPhotosFor(const IdxData* idx, std::vector<PhotoInfo::Ptr>* result)
        {
            for(const IdxData* child: idx->m_children)
            {
                if (child->m_loaded)
                {
                    if (child->m_photo.get() == nullptr)
                        getPhotosFor(child, result);
                    else
                        result->push_back(child->m_photo);
                }
                else
                    assert(!"load not implemented");
            }
        }

        //store or update photo in DB
        void updatePhotoInDB(const PhotoInfo::Ptr& photoInfo)
        {
            if (photoInfo->isLoaded())
            {
                Database::Task task = m_database->prepareTask(this);
                m_database->store(task, photoInfo);
            }
        }

        DBDataModel* pThis;
        IdxData m_root;
        Hierarchy m_hierarchy;
        bool m_dirty;
        Database::IDatabase* m_database;
        Database::PhotoIterator m_iterator;
        ThreadSafeResource<std::unordered_map<Database::Task, std::unique_ptr<ITaskData>, DatabaseTaskHash>> m_db_tasks;

    private:
        //function returns list of tags on particular 'level' for 'parent'
        void getLevelInfo(size_t level, const QModelIndex& _parent)
        {
            if (level + 1 <= m_hierarchy.levels.size())
            {
                std::deque<Database::IFilter::Ptr> filter;

                const TagNameInfo& tagNameInfo = m_hierarchy.levels[level].tagName;
                buildFilterFor(_parent, &filter);
                buildExtraFilters(&filter);

                Database::Task task = m_database->prepareTask(this);
                m_db_tasks.lock().get()[task] = std::unique_ptr<ITaskData>(new ListTagValuesTask(_parent, level));
                m_database->listTagValues(task, tagNameInfo, filter);
            }
        }

        void buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter)
        {
            IdxData* idxData = getParentIdxDataFor(_parent);

            filter->push_back(idxData->m_filter);

            if (idxData->m_level > 0)
                buildFilterFor(_parent.parent(), filter);
        }

        void buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const
        {
            const auto modelSpecificFilters = m_root.m_model->getModelSpecificFilters();
            filter->insert(filter->end(), modelSpecificFilters.begin(), modelSpecificFilters.end());
        }


        virtual void got_getAllPhotos(const Database::Task &, const Database::QueryList &) override
        {
        }

        virtual void got_getPhoto(const Database::Task &, const PhotoInfo::Ptr &) override
        {
        }

        //called when leafs for particual node have been loaded
        virtual void got_getPhotos(const Database::Task & task, const Database::QueryList& photos) override
        {
            auto it = m_db_tasks.lock().get().find(task);
            GetPhotosTask* l_task = static_cast<GetPhotosTask *>(it->second.get());
            IdxData* idxData = getParentIdxDataFor(l_task->m_parent);

            std::shared_ptr<std::deque<IdxData *>> leafs(new std::deque<IdxData *>);

            for(PhotoInfo::Ptr photoInfo: photos)
            {
                IdxData* newItem = new IdxData(pThis, idxData, photoInfo);
                leafs->push_back(newItem);
            }

            m_db_tasks.lock().get().erase(it);

            //attach photos to parent node in main thread
            pThis->attachNodes(idxData, leafs);
        }

        virtual void got_listTags(const Database::Task &, const std::vector<TagNameInfo> &) override
        {
        }

        //called when nodes for particual node have been loaded
        virtual void got_listTagValues(const Database::Task& task, const std::deque<TagValueInfo>& tags) override
        {
            auto it = m_db_tasks.lock().get().find(task);
            ListTagValuesTask* l_task = static_cast<ListTagValuesTask *>(it->second.get());

            const size_t level = l_task->m_level;
            const QModelIndex& _parent = l_task->m_parent;
            IdxData* idxData = getParentIdxDataFor(_parent);

            for(const TagValueInfo& tag: tags)
            {
                auto fdesc = std::make_shared<Database::FilterDescription>();
                fdesc->tagName = m_hierarchy.levels[level].tagName;
                fdesc->tagValue = tag;

                IdxData* newItem = new IdxData(m_root.m_model, idxData, tag);
                newItem->setNodeData(fdesc);

                idxData->addChild(newItem);
            }

            m_db_tasks.lock().get().erase(it);

            //emit signals about update
            pThis->idxUpdated(idxData);
        }


        virtual void got_storeStatus(const Database::Task &) override
        {
            //TODO: some validation?
        }
};


////////////////////////////////////////////////////////////////////////////////


DBDataModel::DBDataModel(QObject* p): QAbstractItemModel(p), m_impl(new Impl(this))
{
    qRegisterMetaType< std::shared_ptr<std::deque<IdxData *>> >("std::shared_ptr<std::deque<IdxData *>>");

    //used for moving notifications to main thread
    connect(this, SIGNAL(dispatchUpdate(IdxData*)), this, SLOT(dispatchIdxUpdate(IdxData*)));
    connect(this, SIGNAL(dispatchNodes(IdxData*,std::shared_ptr<std::deque<IdxData *> >)),
            this, SLOT(dispatchAttachNodes(IdxData*,std::shared_ptr<std::deque<IdxData *> >)));
}


DBDataModel::~DBDataModel()
{

}


void DBDataModel::setHierarchy(const Hierarchy& hierarchy)
{
    m_impl->setHierarchy(hierarchy);
}


PhotoInfo::Ptr DBDataModel::getPhoto(const QModelIndex& idx) const
{
    IdxData* idxData = m_impl->getIdxDataFor(idx);
    return idxData->m_photo;
}


const std::vector<PhotoInfo::Ptr> DBDataModel::getPhotos()
{
    std::vector<PhotoInfo::Ptr> result;
    m_impl->getPhotosFor(&m_impl->m_root, &result);

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
    emit dispatchUpdate(idxData);
}

void DBDataModel::attachNodes(IdxData* _parent, const std::shared_ptr<std::deque<IdxData *>>& leafs)
{
    //make sure, we will move to main thread
    emit dispatchNodes(_parent, leafs);
}


IdxData& DBDataModel::getRootIdxData()
{
    return m_impl->m_root;
}


void DBDataModel::updatePhotoInDB(const PhotoInfo::Ptr& photoInfo)
{
    m_impl->updatePhotoInDB(photoInfo);
}


QModelIndex DBDataModel::createIndex(IdxData* idxData) const
{
    const QModelIndex idx = createIndex(idxData->m_row, idxData->m_column, idxData);
    return idx;
}


void DBDataModel::dispatchIdxUpdate(IdxData* idxData)
{
    QModelIndex idx = createIndex(idxData);
    emit dataChanged(idx, idx);

    //if photo changed, store it in database
    PhotoInfo::Ptr photoInfo = idxData->m_photo;
    if (photoInfo.get() != nullptr)
        m_impl->updatePhotoInDB(photoInfo);
}


void DBDataModel::dispatchAttachNodes(IdxData* _parent, const std::shared_ptr<std::deque<IdxData *>>& photos)
{
    //attach nodes to parent in main thread
    QModelIndex parentIdx = createIndex(_parent);
    const size_t last = photos->size() - 1;
    beginInsertRows(parentIdx, 0, last);

    for(IdxData* newItem: *photos)
        _parent->addChild(newItem);

    endInsertRows();
}
