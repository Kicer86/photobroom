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

IdxDataDeepFetcher::IdxDataDeepFetcher(): m_modelImpl(nullptr), m_notLoaded()
{

}


IdxDataDeepFetcher::~IdxDataDeepFetcher()
{

}


void IdxDataDeepFetcher::setModelImpl(DBDataModelImpl* modelImpl)
{
    m_modelImpl = modelImpl;
}


void IdxDataDeepFetcher::fetch(IdxData* idx)
{
    m_notLoaded.push_back(idx);

    process();
}


void IdxDataDeepFetcher::process()
{
    while(m_notLoaded.empty() == false)
    {
        IdxData* idxData = m_notLoaded.front();
        m_notLoaded.pop_front();

        if (idxData->m_loaded == IdxData::LoadStatus::NotLoaded)
        {
            QModelIndex idx = m_modelImpl->getIndex(idxData);
            m_modelImpl->fetchMore(idx);
        }

    }
}
