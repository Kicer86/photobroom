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

#include "idx_data.hpp"

struct DBDataModel::Impl
{
    Impl(DBDataModel* model): m_root(model, nullptr, "root"), m_hierarchy(), m_dirty(true), m_backend(), m_iterator()
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

    Database::PhotoIterator& getIterator()
    {
        if (m_dirty)
        {
            m_dirty = false;
            Database::QueryList list = m_backend->getAllPhotos();
            m_iterator = list.begin();
        }

        return m_iterator;
    }

    void fetchMore(const QModelIndex& _parent)
    {
        IdxData* idxData = getParentIdxDataFor(_parent);
        const size_t level = idxData->m_level;

        if (level < m_hierarchy.levels.size())  //construct nodes basing on tags
        {
            std::set<TagValueInfo> tags = getLevelInfo(level + 1, _parent);

            for(const TagValueInfo& tag: tags)
            {
                auto fdesc = std::make_shared<Database::FilterDescription>();
                fdesc->tagName = m_hierarchy.levels[level].tagName;
                fdesc->tagValue = tag;

                IdxData* newItem = new IdxData(m_root.m_model, idxData, tag);
                newItem->setNodeData(fdesc);

                idxData->addChild(newItem);
            }
        }
        else if (level == m_hierarchy.levels.size())  //construct leafs basing on photos
        {
            std::vector<Database::IFilter::Ptr> filter;
            buildFilterFor(_parent, &filter);

            Database::QueryList photos = m_backend->getPhotos(filter);

            for(PhotoInfo::Ptr photoInfo: photos)
            {
                IdxData* newItem = new IdxData(m_root.m_model, idxData, photoInfo);
                idxData->addChild(newItem);
            }
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

    void setBackend(Database::IBackend* backend)
    {
        m_backend = backend;
    }

    void close()
    {
        if (m_backend)
            m_backend->closeConnections();
    }

    IdxData* getIdxDataFor(const QModelIndex& obj)
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
            m_backend->store(photoInfo);
    }

    IdxData m_root;
    Hierarchy m_hierarchy;
    bool m_dirty;
    Database::IBackend* m_backend;
    Database::PhotoIterator m_iterator;

    private:
        //function returns set of tags on particular 'level' for 'parent'
        std::set<TagValueInfo> getLevelInfo(size_t level, const QModelIndex& _parent)
        {
            std::set<TagValueInfo> result;

            if (level <= m_hierarchy.levels.size())
            {
                const TagNameInfo& tagNameInfo = m_hierarchy.levels[level - 1].tagName;

                if (level == 1)
                    result = m_backend->listTagValues(tagNameInfo);
            }

            return result;
        }

        void buildFilterFor(const QModelIndex& _parent, std::vector<Database::IFilter::Ptr>* filter)
        {
            IdxData* idxData = getParentIdxDataFor(_parent);

            filter->push_back(idxData->m_filter);

            if (idxData->m_level > 0)
                buildFilterFor(_parent.parent(), filter);
        }
};


////////////////////////////////////////////////////////////////////////////////


DBDataModel::DBDataModel(QObject* p): QAbstractItemModel(p), m_impl(new Impl(this))
{
    // Register QVector of ints - required by queued signals.
    // It is necessary, as model emits dataChanged() signal.
    // And it may happen from various threads as IdxInfo will inform us about photo change,
    // and photo updaters are run in threads.
    // View classes will connect to dataChanged and will make sure it always come from main thread
    // (gui operations are forbiden from non-main thread).
    qRegisterMetaType<QVector<int>>("QVector<int>");
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


void DBDataModel::setBackend(Database::IBackend* backend)
{
    m_impl->setBackend(backend);
}


void DBDataModel::close()
{
    m_impl->close();
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


void DBDataModel::idxUpdated(IdxData* idxData)
{
    QModelIndex idx = createIndex(idxData);
    emit dataChanged(idx, idx);

    PhotoInfo::Ptr photoInfo = idxData->m_photo;
    m_impl->updatePhotoInDB(photoInfo);
}
