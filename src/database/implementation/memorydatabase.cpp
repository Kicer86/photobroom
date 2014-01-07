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

#include <boost/crc.hpp>
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

            Entry entry(photoInfo);

            //entry.m_d->m_crc = calcCrc(path);
            //entry.m_d->m_path = decoratePath(path);

            m_db[entry.m_d->m_crc] = entry;

            registerUpdate(entry.m_d->m_crc);
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

                boost::optional<Entry::crc32> entry = getItemToUpdate();

                if (entry)
                {
                    const Entry &dbEntry = m_db[*entry];
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
            const static int m_max_queue_len = 256;                 //max len of db queue
            std::unordered_map<Entry::crc32, Entry> m_db;           //files managed by database
            std::shared_ptr<IStreamFactory> m_stream;
            Database::IConfiguration *m_configuration;
            std::shared_ptr<IBackend> m_backend;
            std::mutex m_backendMutex;
            std::condition_variable m_backendSet;
            TS_Queue<Entry::crc32> m_updateQueue;                   //entries to be stored in backend
            std::thread m_storekeeper;
            bool m_dbClosed;

            void registerUpdate(const Entry::crc32 &item)
            {
                m_updateQueue.push_back(item);
            }

            boost::optional<Entry::crc32> getItemToUpdate()
            {
                const boost::optional<Entry::crc32> entry = m_updateQueue.pop_front();

                return entry;
            }

            Entry::crc32 calcCrc(const std::string &path) const
            {
                const int MAX_SIZE = 65536;
                boost::crc_32_type crc;
                std::shared_ptr<std::iostream> input = m_stream->openStream(path, std::ios_base::in | std::ios_base::binary);

                if (input != nullptr)
                    do
                    {
                        char buf[MAX_SIZE];

                        input->read(buf, MAX_SIZE);
                        crc.process_bytes(buf, input->gcount());
                    }
                    while(input->fail() == false);

                const Entry::crc32 sum = crc();

                return sum;
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
