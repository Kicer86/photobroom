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

#include <core/base_tags.hpp>
#include <database/query_list.hpp>
#include <unordered_map>


//namespace
//{
    struct IdxData
    {
        int m_level;
        std::vector<IdxData *> m_children;
        QMap<int, QVariant> m_data;
        bool m_loaded;

        IdxData(int level, const QString& name): m_level(level), m_children(), m_data(), m_loaded(false)
        {
            m_data[Qt::DisplayRole] = name;
        }

    };
//}


struct DBDataModel::Impl
{
    Impl(): m_root(0, "root"), m_hierarchy(), m_dirty(true)
    {
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

    void fetchMore(const QModelIndex& parent)
    {
        IdxData* idxData = getParentIdxDataFor(parent);

        std::vector<TagValueInfo> tags = getLevelInfo(idxData->m_level + 1, parent);
        const int level = idxData->m_level;

        for(const TagValueInfo& tag: tags)
            idxData->m_children.push_back(new IdxData(level + 1, tag));

        idxData->m_loaded = true;
    }

    bool canFetchMore(const QModelIndex& parent)
    {
        IdxData* idxData = getParentIdxDataFor(parent);
        const bool status = !idxData->m_loaded;

        return status;
    }

    void setBackend(const std::shared_ptr<Database::IBackend>& backend)
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

    IdxData* getParentIdxDataFor(const QModelIndex& parent)
    {
        IdxData* idxData = getIdxDataFor(parent);

        if (idxData == nullptr)
            idxData = &m_root;

        return idxData;
    }

    private:
        IdxData m_root;

        Hierarchy m_hierarchy;
        bool m_dirty;
        std::shared_ptr<Database::IBackend> m_backend;
        Database::PhotoIterator m_iterator;

        //function returns set of tags on particular 'level' for 'parent'
        std::vector<TagValueInfo> getLevelInfo(int level, const QModelIndex& parent)
        {
            std::vector<TagValueInfo> result;

            if (level <= m_hierarchy.levels.size())
            {
                const TagNameInfo& tagNameInfo = m_hierarchy.levels[level - 1].tagName;

                if (level == 1)
                    result = m_backend->listTagValues(tagNameInfo);
            }

            return result;
        }
};


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


bool DBDataModel::canFetchMore(const QModelIndex& parent) const
{
    return m_impl->canFetchMore(parent);
}


void DBDataModel::fetchMore(const QModelIndex& parent)
{
    m_impl->fetchMore(parent);
}


int DBDataModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}


QVariant DBDataModel::data(const QModelIndex& index, int role) const
{
    IdxData* idxData = m_impl->getIdxDataFor(index);
    const QVariant& v = idxData->m_data[role];

    return v;
}


QModelIndex DBDataModel::index(int row, int column, const QModelIndex& parent) const
{
    IdxData* pData = m_impl->getParentIdxDataFor(parent);
    IdxData* cData = pData->m_children[row];

    QModelIndex idx = createIndex(row, column, cData);

    return idx;
}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}


int DBDataModel::rowCount(const QModelIndex& parent) const
{
    IdxData* idxData = m_impl->getParentIdxDataFor(parent);

    //TODO: rowCount() is called before canFetchMore() and fetchMore(). Is it ok?
    //      load data if data is not loaded yet
    if (idxData->m_loaded == false)
        m_impl->fetchMore(parent);

    const size_t children = idxData->m_children.size();

    return children;
}


bool DBDataModel::addPhoto(const IPhotoInfo::Ptr& photoInfo)
{
    return false;
}


void DBDataModel::setBackend(const std::shared_ptr<Database::IBackend>& backend)
{
    m_impl->setBackend(backend);
}


void DBDataModel::close()
{
    m_impl->close();
}
