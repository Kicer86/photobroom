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

#ifndef DBDATAMODELIMPL_H
#define DBDATAMODELIMPL_H

#include <database/idatabase.hpp>

#include "idx_data.hpp"
#include "../db_data_model.hpp"

class DBDataModel;
class PhotosMatcher;


class IdxDataManager: public QObject
{
public:
    struct INotifications
    {
        virtual ~INotifications();
    };

    IdxDataManager(DBDataModel* model);
    IdxDataManager(const IdxDataManager &) = delete;
    IdxDataManager& operator=(const IdxDataManager &) = delete;

    ~IdxDataManager();

    void setHierarchy(const Hierarchy& hierarchy);
    const Hierarchy& getHierarchy() const;

    void set(ITaskExecutor *);

    void fetchMore(const QModelIndex& _parent);
    void deepFetch(IIdxData* top);
    bool canFetchMore(const QModelIndex& _parent);
    void setDatabase(Database::IDatabase* database);
    void applyFilters(const SearchExpressionEvaluator::Expression &);
    void refetchNode(IdxNodeData *);

    IdxNodeData* getRoot();
    IIdxData* getIdxDataFor(const QModelIndex& obj) const;
    QModelIndex getIndex(IIdxData* idxData) const;
    bool hasChildren(const QModelIndex& _parent);
    IdxNodeData* parent(const QModelIndex& child);

    //signals from IdxData:
    void idxDataCreated(IIdxData *);
    void idxDataDeleted(IdxLeafData *);
    void idxDataDeleted(IdxNodeData *);
    void idxDataReset(IIdxData *);

private:
    Q_OBJECT

    struct Data;
    std::unique_ptr<Data> m_data;

    template<typename T>
    void forIndexChildren(const IdxNodeData* node, const T& action)
    {
        for(const IIdxData::Ptr& child: node->getChildren())
            action(child.get());
    }

    void fetchTagValuesFor(size_t level, const QModelIndex& _parent);
    void fetchPhotosFor(const QModelIndex &);
    void checkForNonmatchingPhotos(size_t level, const QModelIndex& _parent);

    void buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter);
    void buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const;
    void fetchData(const QModelIndex &);
    void setupNewNode(IIdxData *, const Database::IFilter::Ptr &, const Hierarchy::Level &) const;
    void setupRootNode();

    // database custom actions (runned by database thread)
    void getPhotosForParent(Database::IBackendOperator *, const QModelIndex &, const std::deque<Database::IFilter::Ptr> &);

    // database tasks callbacks:
    void gotNonmatchingPhotosForParent(const QModelIndex &, int);
    void gotTagValuesForParent(const QModelIndex &, std::size_t, const std::deque<TagValue> &);
    //

    void markIdxDataFetched(IIdxData *);
    void markIdxDataBeingFetched(IIdxData *);

    void removeIdxDataFromNotFetched(IIdxData *);
    void addIdxDataToNotFetched(IIdxData *);

    void resetModel();

    //model manipulations
    void appendIdxData(IdxNodeData *, const std::shared_ptr<std::deque<IIdxData::Ptr>> &);
    bool movePhotoToRightParent(const IPhotoInfo::Ptr &);
    IdxNodeData* getCurrentParent(const IPhotoInfo::Ptr &);
    IdxNodeData* createAncestry(const IPhotoInfo::Ptr &);                            //returns direct parent or nullptr if direct parent isn't fetched yet
    IIdxData* findIdxDataFor(const IPhotoInfo::Ptr &);
    IIdxData* findIdxDataFor(const Photo::Id &);
    IdxNodeData* createCloserAncestor(PhotosMatcher *, const IPhotoInfo::Ptr &);     //returns direct parent or nullptr if direct parent isn't fetched yet
    IdxNodeData* createUniversalAncestor(PhotosMatcher *, const IPhotoInfo::Ptr &);  //returns pointer to universal ancestor for given photo if could be created
    void removeChildren(IdxNodeData *);                                              // remove all children
    void performMove(const IPhotoInfo::Ptr &, IdxNodeData *, IdxNodeData *);
    void performMove(IIdxData* item, IdxNodeData* from, IdxNodeData* to);
    void performRemoveChildren(IdxNodeData *);
    void performRemove(const IPhotoInfo::Ptr &);
    void performRemove(const Photo::Id &);
    void performRemove(IIdxData *);
    IIdxData* performAdd(const IPhotoInfo::Ptr &, IdxNodeData* parent);
    IIdxData* performAdd(IdxNodeData* parent, IIdxData::Ptr&& child);
    bool sortChildrenOf(IdxNodeData *);
    //

    IIdxData::Ptr prepareUniversalNodeFor(IIdxData *);                           //prepares node for photos without tag required by particular parent

    void insertFetchedNodes(IdxNodeData *, const std::shared_ptr<std::deque<IIdxData::Ptr>> &);

signals:
    void dataChanged(IIdxData *, const QVector<int> &);

private slots:
    // database notifications
    void photoChanged(const IPhotoInfo::Ptr &);
    void photosAdded(const std::deque<IPhotoInfo::Ptr> &);
    void photosRemoved(const std::deque<Photo::Id> &);
    //
};


#endif // DBDATAMODELIMPL_H
