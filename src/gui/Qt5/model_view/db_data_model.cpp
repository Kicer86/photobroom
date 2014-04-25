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

#include <core/base_tags.hpp>
#include <database/query_list.hpp>
#include <database/filter.hpp>


namespace
{
    struct IdxData
    {
        std::vector<IdxData *> m_children;
        QMap<int, QVariant> m_data;
        Database::FilterDescription m_filter;
        IPhotoInfo::Ptr m_photo;
        IdxData* m_parent;
        size_t m_level;
        int m_row;
        int m_column;
        bool m_loaded;                          // true when we have loaded all children of item (if any)

        // node constructor
        IdxData(IdxData* parent, const QString& name): IdxData(parent)
        {
            m_data[Qt::DisplayRole] = name;
        }

        //leaf constructor
        IdxData(IdxData* parent, const IPhotoInfo::Ptr& photo): IdxData(parent)
        {
            m_photo = photo;
            m_data[Qt::DisplayRole] = photo->getPath().c_str();
            m_loaded = true;
        }

        IdxData(const IdxData &) = delete;
        IdxData& operator=(const IdxData &) = delete;

        void setNodeData(const Database::FilterDescription& filter)
        {
            m_filter = filter;
        }

        void setPosition(int row, int col)
        {
            m_row = row;
            m_column = col;
        }

        void addChild(IdxData* child)
        {
            child->setPosition(m_children.size(), 0);
            m_children.push_back(child);
        }

        void addChild(const APhotoInfo::Ptr& photoInfo)
        {
            IdxData* child = new IdxData(this, photoInfo);
            addChild(child);
        }

        private:
            IdxData(IdxData* parent):
                m_children(),
                m_data(),
                m_filter(),
                m_photo(nullptr),
                m_parent(),
                m_level(-1),
                m_row(0),
                m_column(0),
                m_loaded(false)
            {
                m_parent = parent;
                m_level = parent? parent->m_level + 1: 0;
            }
    };
}


struct DBDataModel::Impl
{
    Impl(): m_root(nullptr, "root"), m_hierarchy(), m_dirty(true), m_backend(), m_iterator()
    {
        m_root.setNodeData(Database::FilterDescription()); //called just to mark root as node, not a leaf
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

        setHierarchy(hierarchy);
    }

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
                Database::FilterDescription fdesc;
                fdesc.tagName = m_hierarchy.levels[level].tagName;
                fdesc.tagValue = tag;

                IdxData* newItem = new IdxData(idxData, tag);
                newItem->setNodeData(fdesc);

                idxData->addChild(newItem);
            }
        }
        else if (level == m_hierarchy.levels.size())  //construct leafs basing on photos
        {
            Database::Filter filter;
            buildFilterFor(_parent, &filter);

            Database::QueryList photos = m_backend->getPhotos(filter);

            for(IPhotoInfo::Ptr photoInfo: photos)
            {
                IdxData* newItem = new IdxData(idxData, photoInfo);
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

    void addPhoto(const APhotoInfo::Ptr& photo)
    {
        m_root.addChild(photo);
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

        void buildFilterFor(const QModelIndex& _parent, Database::Filter* filter)
        {
            IdxData* idxData = getParentIdxDataFor(_parent);

            filter->addFilter(idxData->m_filter);

            if (idxData->m_level > 0)
                buildFilterFor(_parent.parent(), filter);
        }
};


////////////////////////////////////////////////////////////////////////////////


DBDataModel::DBDataModel(QObject* p): QAbstractItemModel(p), m_impl(new Impl)
{

}


DBDataModel::~DBDataModel()
{

}


void DBDataModel::setHierarchy(const Hierarchy& hierarchy)
{
    m_impl->setHierarchy(hierarchy);
}


IPhotoInfo::Ptr DBDataModel::getPhoto(const QModelIndex& idx) const
{
    IdxData* idxData = m_impl->getIdxDataFor(idx);
    return idxData->m_photo;
}


const std::vector< IPhotoInfo::Ptr > DBDataModel::getPhotos()
{

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

    return idx;
}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{
    IdxData* idxData = m_impl->parent(child);
    QModelIndex parentIdx = idxData? createIndex(idxData->m_row, idxData->m_column, idxData):
                                     QModelIndex();

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


bool DBDataModel::addPhoto(const IPhotoInfo::Ptr& photoInfo)
{
    const int row = m_impl->m_root.m_children.size();

    beginInsertRows(QModelIndex(), row, row);
    m_impl->addPhoto(photoInfo);
    endInsertRows();

    return true;
}


void DBDataModel::setBackend(Database::IBackend* backend)
{
    m_impl->setBackend(backend);
}


void DBDataModel::close()
{
    m_impl->close();
}
