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
#include <mutex>
#include <set>
#include <condition_variable>

#include <QObject>

#include <core/task_executor.hpp>

class QEventLoopLocker;
class QModelIndex;

class IdxDataManager;
class IdxData;
struct IIdxData;

class IdxDataDeepFetcher: public QObject, public ITaskExecutor::ITask
{
        Q_OBJECT

    public:
        IdxDataDeepFetcher();
        IdxDataDeepFetcher(const IdxDataDeepFetcher& other) = delete;
        ~IdxDataDeepFetcher();
        IdxDataDeepFetcher& operator=(const IdxDataDeepFetcher& other) = delete;

        void setModelImpl(IdxDataManager *);
        void setIdxDataToFetch(IdxData* idx);
        void setEventLoopLocker(QEventLoopLocker *);

    private:
        IdxDataManager* m_idxDataManager;
        QEventLoopLocker* m_eventLoopLocker;
        std::deque<IdxData *> m_notLoaded;               //nodes not fetched yet
        std::set<IIdxData *> m_inProcess;                 //nodes being fetched at this moment
        std::mutex m_idxDataMutex;
        std::condition_variable m_dataNotifier;

        void process();
        void process(IIdxData *);
        void idxDataLoaded(IdxData *);

        //ITask:
        virtual std::string name() const override;
        virtual void perform() override;

    private slots:
        void dataChanged(IdxData *, const QVector<int> &);
};

#endif // IDXDATADEEPFETCHER_HPP
