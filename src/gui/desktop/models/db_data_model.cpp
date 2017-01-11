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

#include <core/cross_thread_call.hpp>
#include <database/filter.hpp>

#include "model_helpers/idx_data.hpp"
#include "model_helpers/idx_data_manager.hpp"


struct DBDataModel::Grouper
{
    Grouper(Database::IDatabase* db):
        m_doneCallback(),
        m_photos(),
        m_db(db),
        m_grp_id(0),
        m_flags(0)
    {

    }

    Grouper(const Grouper &) = delete;
    Grouper& operator=(const Grouper &) = delete;

    void setDoneCallback(const std::function<void()>& doneCallback)
    {
        m_doneCallback = doneCallback;
    }

    void create(const std::vector<Photo::Id>& photos, const Photo::Id& representativePhoto)
    {
        std::function<void(const Group::Id &)> group_created =
            std::bind(&Grouper::groupCreated, this, std::placeholders::_1);

        std::function<void(const std::deque<IPhotoInfo::Ptr> &)> photos_received =
            std::bind(&Grouper::photosReceived, this, std::placeholders::_1);

        m_db->createGroup(representativePhoto, group_created);
        m_db->getPhotos(photos, photos_received);
    }

    private:
        std::function<void()> m_doneCallback;
        std::deque<IPhotoInfo::Ptr> m_photos;
        Database::IDatabase* m_db;
        Group::Id m_grp_id;

        enum Flags
        {
            GotGroupId         = 1,
            GotPhotos          = 2
        };

        int m_flags;

        void groupCreated(const Group::Id& id)
        {
            assert( (m_flags & GotGroupId) == 0 );

            m_grp_id = id;
            m_flags |= GotGroupId;

            gotData();
        }

        void photosReceived(const std::deque<IPhotoInfo::Ptr>& photos)
        {
            assert( (m_flags & GotPhotos) == 0 );

            m_photos = photos;
            m_flags |= GotPhotos;

            gotData();
        }

        void gotData()
        {
            if ( m_flags == (GotGroupId | GotPhotos) )
            {
                for(const IPhotoInfo::Ptr& photoInfo: m_photos)
                    photoInfo->setGroup(m_grp_id);

                m_doneCallback();
            }
        }
};


Hierarchy::Level::Level(): tagName(), order()
{

}


Hierarchy::Level::Level(const TagNameInfo& i, const Hierarchy::Level::Order& o): tagName(i), order(o)
{

}


Hierarchy::Hierarchy(): levels()
{

}


Hierarchy::Hierarchy(const std::initializer_list<Hierarchy::Level>& l): levels(l)
{

}


size_t Hierarchy::nodeLevels() const
{
    return levels.size() - 1;        // last level is for leafs description
}


const Hierarchy::Level& Hierarchy::getNodeInfo(size_t level) const
{
    assert(level < levels.size());   // less than real size of levels?

    return levels[level];
}


const Hierarchy::Level& Hierarchy::getLeafsInfo() const
{
    const size_t last = levels.size() - 1;

    return levels[last];
}


//////////////////////////////////////


DBDataModel::DBDataModel(QObject* p):
    APhotoInfoModel(p),
    m_idxDataManager(new IdxDataManager(this)),
    m_database(nullptr),
    m_filters(),
    m_groupers()
{
    connect(m_idxDataManager.get(), &IdxDataManager::dataChanged, this, &DBDataModel::itemDataChanged);
}


DBDataModel::~DBDataModel()
{

}


IPhotoInfo::Ptr DBDataModel::getPhoto(const QModelIndex& idx) const
{
    IIdxData* idxData = m_idxDataManager->getIdxDataFor(idx);
    return idxData->getPhoto();
}


const std::deque<Database::IFilter::Ptr>& DBDataModel::getStaticFilters() const
{
    return m_filters;
}


bool DBDataModel::isEmpty() const
{
    IIdxData* root = m_idxDataManager->getRoot();
    const bool result = root->getChildren().empty();

    return result;
}


void DBDataModel::deepFetch(const QModelIndex& top)
{
    IIdxData* idx = m_idxDataManager->getIdxDataFor(top);
    m_idxDataManager->deepFetch(idx);
}


void DBDataModel::group(const std::vector<Photo::Id>& photos, const QString& representativePath)
{
    using namespace std::placeholders;

    std::function<void(const std::vector<Photo::Id> &)> store_callback =
        [this, photos](const std::vector<Photo::Id>& id)
        {
            assert(id.size() == 1);

            if (id.empty() == false)
                group(photos, id.front());
        };

    auto this_tread_callback = make_cross_thread_function(this, store_callback);

    const Photo::FlagValues flags = {
            {Photo::FlagsE::Role,        static_cast<int>(Photo::Roles::Representative)},
            {Photo::FlagsE::StagingArea, 1}
    };

    m_database->store({representativePath}, flags, this_tread_callback);
}


void DBDataModel::group(const std::vector<Photo::Id>& photos, const Photo::Id& representativePhoto)
{
    const auto emplaced = m_groupers.emplace( std::make_unique<Grouper>(m_database) );
    const auto grouperIt = emplaced.first;
    const std::unique_ptr<Grouper>& grouper = *grouperIt;

    std::function<void()> doneCallbackFun = [this, grouperIt]()
    {
        m_groupers.erase(grouperIt);
    };

    auto doneCallback = make_cross_thread_function(this, doneCallbackFun);

    grouper->setDoneCallback(doneCallback);
    grouper->create(photos, representativePhoto);
}


void DBDataModel::setHierarchy(const Hierarchy& hierarchy)
{
    m_idxDataManager->setHierarchy(hierarchy);
}



IPhotoInfo* DBDataModel::getPhotoInfo(const QModelIndex& idx) const
{
    IIdxData* idxData = m_idxDataManager->getIdxDataFor(idx);
    return idxData->getPhoto().get();
}


bool DBDataModel::canFetchMore(const QModelIndex& _parent) const
{
    const bool status = m_database == nullptr? false: m_idxDataManager->canFetchMore(_parent);

    return status;
}


void DBDataModel::fetchMore(const QModelIndex& _parent)
{
    m_idxDataManager->fetchMore(_parent);
}


int DBDataModel::columnCount(const QModelIndex &) const
{
    return 1;
}


QVariant DBDataModel::data(const QModelIndex& _index, int role) const
{
    QVariant v = APhotoInfoModel::data(_index, role);

    if (v.isNull())
    {
        IIdxData* idxData = m_idxDataManager->getIdxDataFor(_index);
        v = idxData->getData(role);
    }

    return v;
}


QModelIndex DBDataModel::index(int row, int column, const QModelIndex& _parent) const
{
    assert(row >= 0);
    const unsigned int urow = static_cast<unsigned int>(row);

    QModelIndex idx;
    IIdxData* pData = m_idxDataManager->getIdxDataFor(_parent);

    if (urow < pData->getChildren().size())             //row out of boundary?
    {
        const std::vector<IIdxData::Ptr>& children = pData->getChildren();
        IIdxData* cData = children[urow].get();
        idx = createIndex(row, column, cData);
    }

    return idx;
}


QModelIndex DBDataModel::parent(const QModelIndex& child) const
{
    IIdxData* idxData = m_idxDataManager->parent(child);
    QModelIndex parentIdx = idxData? createIndex(idxData): QModelIndex();

    return parentIdx;
}


int DBDataModel::rowCount(const QModelIndex& _parent) const
{
    IIdxData* idxData = m_idxDataManager->getIdxDataFor(_parent);
    const size_t count = idxData->getChildren().size();

    assert(count < std::numeric_limits<int>::max());
    return static_cast<int>(count);
}


bool DBDataModel::hasChildren(const QModelIndex& _parent) const
{
    return m_idxDataManager->hasChildren(_parent);
}


void DBDataModel::setDatabase(Database::IDatabase* database)
{
    m_idxDataManager->setDatabase(database);
    m_database = database;
}


void DBDataModel::set(ITaskExecutor* taskExecutor)
{
    m_idxDataManager->set(taskExecutor);
}


void DBDataModel::setStaticFilters(const std::deque<Database::IFilter::Ptr>& filters)
{
    m_filters = filters;

    IIdxData* root = m_idxDataManager->getRoot();
    m_idxDataManager->refetchNode(root);
}


void DBDataModel::applyFilters(const SearchExpressionEvaluator::Expression& filters)
{
    m_idxDataManager->applyFilters(filters);
}


IIdxData* DBDataModel::getRootIdxData()
{
    return m_idxDataManager->getRoot();
}


QModelIndex DBDataModel::createIndex(IIdxData* idxData) const
{
    const QModelIndex idx = idxData->getLevel() == 0? QModelIndex():          //level 0 == parent of all parents represented by invalid index
                                                      createIndex(idxData->getRow(), idxData->getCol(), idxData);
    return idx;
}


void DBDataModel::itemDataChanged(IIdxData* idxData, const QVector<int>& roles)
{
    const QModelIndex idx = m_idxDataManager->getIndex(idxData);

    emit dataChanged(idx, idx, roles);
}
