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
    void deepFetch(IdxData* top);
    bool canFetchMore(const QModelIndex& _parent);
    void setDatabase(Database::IDatabase* database);
    void applyFilters(const SearchExpressionEvaluator::Expression &);
    void refetchNode(IdxData *);

    IdxData* getRoot();
    IdxData* getIdxDataFor(const QModelIndex& obj) const;
    QModelIndex getIndex(IdxData* idxData) const;
    bool hasChildren(const QModelIndex& _parent);
    IdxData* parent(const QModelIndex& child);
    void getPhotosFor(const IdxData* idx, std::vector<IPhotoInfo::Ptr>* result);

    //signals from IdxData:
    void idxDataCreated(IdxData *);
    void idxDataDeleted(IdxData *);
    void idxDataReset(IdxData *);

private:
    Q_OBJECT

    struct Data;
    std::unique_ptr<Data> m_data;

    template<typename T>
    void forIndexChildren(const IdxData* index, const T& action)
    {
        for(const IdxData* child: index->m_children)
            action(child);
    }

    void fetchTagValuesFor(size_t level, const QModelIndex& _parent);
    void fetchPhotosFor(const QModelIndex &);
    void checkForNonmatchingPhotos(size_t level, const QModelIndex& _parent);

    void buildFilterFor(const QModelIndex& _parent, std::deque<Database::IFilter::Ptr>* filter);
    void buildExtraFilters(std::deque<Database::IFilter::Ptr>* filter) const;
    void fetchData(const QModelIndex &);
    void setupNewNode(IdxData *, const Database::IFilter::Ptr &, const Hierarchy::Level &) const;
    void setupRootNode();

    // database tasks callbacks:
    void gotPhotosForParent(const QModelIndex &, const IPhotoInfo::List& photos);
    void gotNonmatchingPhotosForParent(const QModelIndex &, int);
    void gotTagValuesForParent(const QModelIndex &, std::size_t, const std::deque<TagValue> &);
    //

    void markIdxDataFetched(IdxData *);
    void markIdxDataBeingFetched(IdxData *);

    void removeIdxDataFromNotFetched(IdxData *);
    void addIdxDataToNotFetched(IdxData *);

    void resetModel();

    //model manipulations
    void appendIdxData(IdxData *, const std::deque<IdxData *> &);
    bool movePhotoToRightParent(const IPhotoInfo::Ptr &);
    IdxData* getCurrentParent(const IPhotoInfo::Ptr &);
    IdxData* createAncestry(const IPhotoInfo::Ptr &);                            //returns direct parent or nullptr if direct parent isn't fetched yet
    IdxData* findIdxDataFor(const IPhotoInfo::Ptr &);
    IdxData* findIdxDataFor(const Photo::Id &);
    IdxData* createCloserAncestor(PhotosMatcher *, const IPhotoInfo::Ptr &);     //returns direct parent or nullptr if direct parent isn't fetched yet
    IdxData* createUniversalAncestor(PhotosMatcher *, const IPhotoInfo::Ptr &);  //returns pointer to universal ancestor for given photo if could be created
    void removeChildren(IdxData *);                                              // remove all children
    void performMove(const IPhotoInfo::Ptr &, IdxData *, IdxData *);
    void performMove(IdxData* item, IdxData* from, IdxData* to);
    void performRemoveChildren(IdxData *);
    void performRemove(const IPhotoInfo::Ptr &);
    void performRemove(const Photo::Id &);
    void performRemove(IdxData *);
    void performAdd(const IPhotoInfo::Ptr &, IdxData *);
    void performAdd(IdxData* parent, IdxData* item);
    bool sortChildrenOf(IdxData *);
    //

    IdxData* prepareUniversalNodeFor(IdxData *);                                 //prepares node for photos without tag required by particular parent

signals:
    void dataChanged(IdxData *, const QVector<int> &);
    void nodesFetched(IdxData *, const std::shared_ptr<std::deque<IdxData *>> &);

private slots:
    // threads synchronization
    void insertFetchedNodes(IdxData *, const std::shared_ptr<std::deque<IdxData *>> &);

    // database notifications
    void photoChanged(const IPhotoInfo::Ptr &);
    void photosAdded(const std::deque<IPhotoInfo::Ptr> &);
    void photosRemoved(const std::deque<Photo::Id> &);
    //
};


#endif // DBDATAMODELIMPL_H
