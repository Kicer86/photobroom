/*
    Database for photos
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "memorydatabase.hpp"

#include <assert.h>

#include <unordered_map>
#include <string>
#include <deque>
#include <thread>
#include <condition_variable>
#include <mutex>

#include <boost/optional.hpp>

#include <OpenLibrary/palgorithm/ts_queue.hpp>

#include "entry.hpp"
#include "ifs.hpp"

namespace Database
{
    namespace
    {
        void trampoline(MemoryDatabase::Impl *);
    }

    struct MemoryDatabase::Impl
    {

        Impl(const std::shared_ptr<IStreamFactory> &stream):
            m_db(),
            m_stream(stream),
            m_configuration(0),
            m_backend(nullptr),
            m_backendMutex(),
            m_backendSet(),
            m_updateQueue(m_max_queue_len),
            m_storekeeper(trampoline, this),
            m_dbClosed(false)
        {

        }


        virtual ~Impl()
        {
            assert(m_dbClosed);

            if (!m_dbClosed)
                close();             //should be done manually due to possible Qt bug: https://bugreports.qt-project.org/browse/QTBUG-35977

            // some interesting information:
            // http://stackoverflow.com/questions/13999432/stdthread-terminate-called-without-an-active-exception-dont-want-to-joi
        }


        Impl(const MemoryDatabase::Impl &) = delete;
        Impl& operator=(const Impl &) = delete;

        void add(const APhotoInfo::Ptr& photoInfo)
        {
            //TODO: check for db opened

            const APhotoInfo::Hash hash = photoInfo->getHash();
            assert(hash.empty() == false);

            m_db[hash] = photoInfo;
            registerUpdate(hash);
        }


        std::string decoratePath(const std::string &path) const
        {
            return std::string("file://") + path;
        }


        void setBackend(const std::shared_ptr<IBackend> &b)
        {
            m_backend = b;

            if (m_backend != nullptr)
                m_backendSet.notify_all();
        }


        void storekeeper()      //storekeeper thread
        {
            while (true)        //work forever
            {
                std::unique_lock<std::mutex> lock(m_backendMutex);

                while(m_backend == nullptr)
                    m_backendSet.wait(lock, [&]{ return m_backend != nullptr; } );      //wait for signal if no backend

                boost::optional<APhotoInfo::Hash> entry = getItemToUpdate();

                if (entry)
                {
                    const APhotoInfo::Hash& hash = *entry;
                    const APhotoInfo::Ptr&  dbEntry = m_db[hash];
                    m_backend->store(dbEntry);
                }
                else
                    break;                                      //the only reason for empty queue is that we are quiting
            }
        }

        void close()
        {
            //flush data
            m_updateQueue.break_popping();

            assert(m_storekeeper.joinable());
            m_storekeeper.join();       //wait for quit

            //close db
            m_backend->closeConnections();

            m_dbClosed = true;
        }

        private:
            const static int m_max_queue_len = 256;                      //max len of db queue
            std::unordered_map<APhotoInfo::Hash, APhotoInfo::Ptr> m_db;  //files managed by database
            std::shared_ptr<IStreamFactory> m_stream;
            Database::IConfiguration *m_configuration;
            std::shared_ptr<IBackend> m_backend;
            std::mutex m_backendMutex;
            std::condition_variable m_backendSet;
            TS_Queue<APhotoInfo::Hash> m_updateQueue;               //entries to be stored in backend
            std::thread m_storekeeper;
            bool m_dbClosed;

            void registerUpdate(const APhotoInfo::Hash &item)
            {
                m_updateQueue.push_back(item);
            }

            boost::optional<APhotoInfo::Hash> getItemToUpdate()
            {
                const boost::optional<APhotoInfo::Hash> entry = m_updateQueue.pop_front();

                return entry;
            }
    };

    namespace
    {
        void trampoline(MemoryDatabase::Impl *impl)
        {
            impl->storekeeper();
        }
    }


    MemoryDatabase::MemoryDatabase(const std::shared_ptr<IStreamFactory>& stream): m_impl(new Impl(stream) )
    {

    }


    MemoryDatabase::~MemoryDatabase()
    {

    }


    bool MemoryDatabase::addPhoto(const APhotoInfo::Ptr& photo)
    {
        m_impl->add(photo);

        return true;
    }


    void MemoryDatabase::setBackend(const std::shared_ptr<Database::IBackend> &backend)
    {
        m_impl->setBackend(backend);
    }


    void MemoryDatabase::close()
    {
        m_impl->close();
    }
}
