/*
 * DBDataModel helper class for deep fetch of data
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

#include "idxdata_deepfetcher.hpp"

#include <QEventLoopLocker>

#include "idx_data.hpp"
#include "idx_data_manager.hpp"


IdxDataDeepFetcher::IdxDataDeepFetcher(): m_idxDataManager(nullptr), m_eventLoopLocker(nullptr), m_notLoaded(), m_inProcess(), m_idxDataMutex(), m_dataNotifier()
{

}


IdxDataDeepFetcher::~IdxDataDeepFetcher()
{
    delete m_eventLoopLocker;  //release event loop locker
}


void IdxDataDeepFetcher::setModelImpl(IdxDataManager* modelImpl)
{
    m_idxDataManager = modelImpl;

    //direct connection is required as signals will come from other threads and we don't have EventLoop here
    connect(m_idxDataManager, &IdxDataManager::dataChanged, this, &IdxDataDeepFetcher::dataChanged, Qt::DirectConnection);
}


void IdxDataDeepFetcher::setIdxDataToFetch(IIdxData* idx)
{
    m_notLoaded.push_back(idx);
}


void IdxDataDeepFetcher::setEventLoopLocker(QEventLoopLocker* eventLoopLocker)
{
    m_eventLoopLocker = eventLoopLocker;
}


void IdxDataDeepFetcher::process()
{
    while(m_notLoaded.empty() == false)
    {
        IIdxData* idxData = m_notLoaded.front();
        m_notLoaded.pop_front();

        process(idxData);
    }
}


void IdxDataDeepFetcher::process(IIdxData* idxData)
{
    // Make sure status of idxData will not change during switch
    std::unique_lock<std::mutex> lock(m_idxDataMutex);

    switch(idxData->status())
    {
        case NodeStatus::NotFetched:
        {
            m_inProcess.insert(idxData);
            QModelIndex idx = m_idxDataManager->getIndex(idxData);
            m_idxDataManager->fetchMore(idx);
            break;
        }

        case NodeStatus::Fetched:
            lock.unlock();                                //we will go recursive now, and we do not need lock anymore in current context
            for(const IIdxData::Ptr& child: idxData->getChildren())
                process(child.get());
            break;

        case NodeStatus::Fetching:
            // Push to m_inProcess queue.
            // m_inProcess is locked by us
            m_inProcess.insert(idxData);
            break;
    }
}


void IdxDataDeepFetcher::idxDataLoaded(IIdxData* idx_data)
{
    // Remove idxData from set of awaiting items.
    // Lock mutex to be sure we won't interference with process()
    std::unique_lock<std::mutex> lock(m_idxDataMutex);

    m_notLoaded.push_back(idx_data);                    //children are certainly not loaded
    m_inProcess.erase(idx_data);

    m_dataNotifier.notify_all();
}


std::string IdxDataDeepFetcher::name() const
{
    return "IdxDataDeepFetcher";
}


void IdxDataDeepFetcher::perform()
{
    bool work = true;

    do
    {
        process();

        std::unique_lock<std::mutex> lock(m_idxDataMutex);
        m_dataNotifier.wait(lock, [&]()
        {
            // Wait as long as there is nothing to process (m_notLoaded empty) and
            // there is still something to appear (m_inProcess not empty).
            // Otherwise quit
            return ! (m_notLoaded.empty() && m_inProcess.empty() == false);
        }
        );

        //still something to do?
        work = m_inProcess.empty() == false || m_notLoaded.empty() == false;
    }
    while(work);
}


void IdxDataDeepFetcher::dataChanged(IIdxData* idxData, const QVector<int>& roles)
{
    auto f = std::find(roles.begin(), roles.end(), DBDataModel::NodeStatus);

    if (f != roles.end())
    {
        const QVariant statusRaw = idxData->getData(DBDataModel::NodeStatus);
        NodeStatus status = static_cast<NodeStatus>(statusRaw.toInt());

        if (status == NodeStatus::Fetched)
            idxDataLoaded(idxData);
    }
}
