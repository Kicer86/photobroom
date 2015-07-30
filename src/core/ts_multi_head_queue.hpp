/*
 * TS_Queue with support for independend subqueues
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef TS_MULTIHEADQUEUE_HPP
#define TS_MULTIHEADQUEUE_HPP

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <deque>
#include <set>
#include <memory>
#include <mutex>


#include <OpenLibrary/utils/optional.hpp>


template<typename T>
class TS_MultiHeadQueue
{
    public:

        class Producer
        {
            public:
                ~Producer()
                {
                    m_queue->release(this);
                }

                void push(const T& item)
                {
                    std::lock_guard<std::recursive_mutex> lock(m_dataMutex);
                    m_data.push_back(item);

                    m_queue->not_empty(this);
                }

                void push(T&& item)
                {
                    std::lock_guard<std::recursive_mutex> lock(m_dataMutex);
                    m_data.push_back(std::move(item));

                    m_queue->not_empty(this);
                }

                void clear()
                {
                    std::lock_guard<std::recursive_mutex> lock(m_dataMutex);
                    m_data.clear();
                }

                std::size_t size() const
                {
                    std::lock_guard<std::recursive_mutex> lock(m_dataMutex);
                    const std::size_t s = m_data.size();

                    return s;
                }

                bool empty() const
                {
                    std::lock_guard<std::recursive_mutex> lock(m_dataMutex);
                    const bool e = m_data.empty();

                    return e;
                }

                std::chrono::time_point<std::chrono::steady_clock> last_access_time() const
                {
                    return m_last_access_time;
                }

            private:
                template<typename> friend class ::TS_MultiHeadQueue;

                TS_MultiHeadQueue<T>* m_queue;
                std::deque<T> m_data;
                mutable std::recursive_mutex m_dataMutex;
                std::chrono::time_point<std::chrono::steady_clock> m_last_access_time;

                Producer(TS_MultiHeadQueue<T>* queue): m_queue(queue), m_data(), m_dataMutex(), m_last_access_time()
                {
                    update_time();
                }

                std::unique_lock<std::mutex> lock()
                {
                    return std::unique_lock<std::recursive_mutex>(m_dataMutex);
                }

                T pop()
                {
                    std::lock_guard<std::recursive_mutex> lock(m_dataMutex);

                    assert(m_data.empty() == false);

                    T v = std::move(m_data.front());
                    m_data.pop_front();

                    update_time();

                    return std::move(v);
                }

                void update_time()
                {
                    m_last_access_time = std::chrono::steady_clock::now();
                }
        };


        TS_MultiHeadQueue(): m_stopped(false)
        {
        }

        TS_MultiHeadQueue(const TS_MultiHeadQueue &) = delete;

        ~TS_MultiHeadQueue()
        {
        }

        TS_MultiHeadQueue& operator=(const TS_MultiHeadQueue &) = delete;

        std::unique_ptr<Producer> prepareProducer()
        {
            std::lock_guard<std::mutex> lock(m_producersMutex);

            std::unique_ptr<Producer> result(new Producer(this));
            m_producers.insert(result.get());

            return std::move(result);
        }

        ol::Optional<T> pop()
        {
            // lock non empty producers
            std::lock_guard<std::mutex> lock(m_non_empty_mutex);
            ol::Optional<T> result;

            wait_for_data(lock);

            if (m_non_empty.empty() == false)
            {
                Producer* top = *m_non_empty.begin();
                result = std::move( top->pop() );

                poped(top);
            }

            return std::move(result);
        }

        ol::Optional<T> pop_for(const std::chrono::milliseconds& timeout)
        {
            // lock non empty producers
            std::unique_lock<std::mutex> lock(m_non_empty_mutex);
            ol::Optional<T> result;

            wait_for_data(lock, timeout);

            if (m_non_empty.empty() == false)
            {
                Producer* top = *m_non_empty.begin();
                result = std::move( top->pop() );

                poped(top);
            }

            return std::move(result);
        }

        void wait_for_data()
        {
            std::unique_lock<std::mutex> lock(m_non_empty_mutex);

            wait_for_data(lock);
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(m_non_empty_mutex);

            return m_non_empty.empty();
        }

        void stop()
        {
            m_stopped = true;
            m_is_not_empty.notify_all();
        }

    private:
        friend class Producer;

        struct ProducerSorter
        {
            bool operator()(Producer* p1, Producer* p2) const
            {
                const auto& t1 = p1->last_access_time();
                const auto& t2 = p2->last_access_time();

                return t1 < t2;
            }
        };

        std::set<Producer *> m_producers;
        std::mutex m_producersMutex;

        std::set<Producer *, ProducerSorter> m_non_empty;
        std::mutex m_non_empty_mutex;

        std::condition_variable m_is_not_empty;

        std::atomic<bool> m_stopped;

        void release(Producer* producer)
        {
            std::lock_guard<std::mutex> lock(m_producersMutex);
            m_producers.erase(producer);
        }

        void not_empty(Producer* producer)
        {
            std::lock_guard<std::mutex> lock(m_non_empty_mutex);

            m_non_empty.insert(producer);
            m_is_not_empty.notify_one();
        }

        void poped(Producer* producer)
        {
            if (producer->empty())
                m_non_empty.erase(producer);
        }

        //! Wait until data is available.
        void wait_for_data(std::unique_lock<std::mutex>& lock)
        {
            auto precond = [&]
            {
                const bool condition = m_stopped == true || m_non_empty.empty() == false;
                return condition;
            };

            m_is_not_empty.wait(lock, precond);
        }

        bool wait_for_data(std::unique_lock<std::mutex>& lock, const std::chrono::milliseconds& timeout)
        {
            auto precond = [&]
            {
                const bool condition = m_stopped == true || m_non_empty.empty() == false;
                return condition;
            };

            const bool status = m_is_not_empty.wait_for(lock, timeout, precond);

            return status;
        }
};

#endif // TS_MULTIHEADQUEUE_H
