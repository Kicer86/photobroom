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

#ifndef IDXDATADEEPFETCHER_H
#define IDXDATADEEPFETCHER_H

#include <deque>

class QModelIndex;

struct DBDataModelImpl;
struct IdxData;

class IdxDataDeepFetcher
{
    public:
        IdxDataDeepFetcher();
        IdxDataDeepFetcher(const IdxDataDeepFetcher& other) = delete;
        ~IdxDataDeepFetcher();
        IdxDataDeepFetcher& operator=(const IdxDataDeepFetcher& other) = delete;

        void setModelImpl(DBDataModelImpl *);
        void fetch(IdxData* idx);                        //make take long. Run in thread

    private:
        DBDataModelImpl* m_modelImpl;
        std::deque<IdxData *> m_notLoaded;

        void process();
};

#endif // IDXDATADEEPFETCHER_H
