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


struct DBDataModel::Impl
{
    Impl(): m_hierarchy(), m_dirty(true)
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

    bool canFetchMore(const QModelIndex& parent)
    {
        const quintptr id = parent.internalId();
        if (m_indicesData.find(id) == m_indicesData.end())  //index not in database? It must be the root!
        {
            //fill first level data
        }

        return static_cast<bool>(getIterator());
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

    private:
        struct IdxData
        {
        };

        std::unordered_map<quintptr, IdxData> m_indicesData;
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
        }
};


DBDataModel::DBDataModel(): QAbstractItemModel(), m_impl(new Impl)
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

}


int DBDataModel::columnCount(const QModelIndex& parent) const
{

}


QVariant DBDataModel::data(const QModelIndex& index, int role) const
{

}


QModelIndex DBDataModel::index(int row, int column, const QModelIndex& parent) const
{

}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{

}


int DBDataModel::rowCount(const QModelIndex& parent) const
{

}


bool DBDataModel::addPhoto(const IPhotoInfo::Ptr& photoInfo)
{

}


void DBDataModel::setBackend(const std::shared_ptr<Database::IBackend>& backend)
{
    m_impl->setBackend(backend);
}


void DBDataModel::close()
{
    m_impl->close();
}
