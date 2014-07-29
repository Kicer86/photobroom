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


#include "idx_data.hpp"
#include "db_data_model_impl.hpp"


IdxDataDeepFetcher::IdxDataDeepFetcher(): m_modelImpl(nullptr), m_notLoaded(), m_inProcess(), m_idxDataMutex(), m_dataNotifier()
{

}


IdxDataDeepFetcher::~IdxDataDeepFetcher()
{

}


void IdxDataDeepFetcher::setModelImpl(IdxDataManager* modelImpl)
{
    m_modelImpl = modelImpl;

    //direct connection is required as signals will come from other threads and we don't have EventLoop here
    connect(m_modelImpl, SIGNAL(idxDataLoaded(IdxData*)), this, SLOT(idxDataLoaded(IdxData*)), Qt::DirectConnection);
}


void IdxDataDeepFetcher::fetch(IdxData* idx)
{
    m_notLoaded.push_back(idx);
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


void IdxDataDeepFetcher::process()
{
    while(m_notLoaded.empty() == false)
    {
        IdxData* idxData = m_notLoaded.front();
        m_notLoaded.pop_front();

        process(idxData);
    }
}


void IdxDataDeepFetcher::process(IdxData* idxData)
{
    // Make sure status of idxData will not change during switch
    std::unique_lock<std::mutex> lock(m_idxDataMutex);

    switch(idxData->m_loaded)
    {
        case IdxData::LoadStatus::NotLoaded:
        {
            m_inProcess.insert(idxData);
            QModelIndex idx = m_modelImpl->getIndex(idxData);
            m_modelImpl->fetchMore(idx);
            break;
        }

        case IdxData::LoadStatus::Loaded:
            lock.unlock();                                //we will go recursive now, and we do not need lock anymore in current context
            for(IdxData* child: idxData->m_children)
                process(child);
            break;

        case IdxData::LoadStatus::Loading:
            // Push to m_inProcess queue.
            // m_inProcess is locked by us
            m_inProcess.insert(idxData);
            break;
    }
}


void IdxDataDeepFetcher::idxDataLoaded(IdxData* idx_data)
{
    // Remove idxData from set of awaiting items.
    // Lock mutex to be sure we won't interference with process()
    std::unique_lock<std::mutex> lock(m_idxDataMutex);

    m_notLoaded.push_back(idx_data);                    //children are certainly not loaded
    m_inProcess.erase(idx_data);

    m_dataNotifier.notify_all();
}

