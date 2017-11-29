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

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <deque>
#include <set>
#include <memory>
#include <mutex>
#include <optional>


template<typename T>
class TS_MultiHeadQueue
{
    public:

        class Producer
        {
            public:
                Producer(const Producer &) = delete;

                ~Producer()
                {
                    m_queue->release(this);
                }

                Producer& operator=(const Producer &) = delete;

                void push(const T& item)
                {
                    std::unique_lock<std::mutex> lock(m_dataMutex);
                    m_data.push_back(item);

                    // Unlock data to avoid deadlocks.
                    // Another thread may wait for data and therefore may lock m_non_empty_mutex and wait for m_dataMutex,
                    // while we have locked m_dataMutex and will be waiting for m_non_empty_mutex in not_empty().

                    lock.unlock();

                    m_queue->not_empty(this);
                }

                void push(T&& item)
                {
                    m_dataMutex.lock();
                    m_data.push_back(std::move(item));

                    // Unlock data to avoid deadlocks.
                    // Another thread may wait for data and therefore may lock m_non_empty_mutex and wait for m_dataMutex,
                    // while we have locked m_dataMutex and will be waiting for m_non_empty_mutex in not_empty().

                    m_dataMutex.unlock();

                    m_queue->not_empty(this);
                }

                void clear()
                {
                    // Inform Queue 'this' is going to be empty,
                    // so Queue will remove 'this' from non-empty list
                    m_queue->aboutToBeCleaned(this);

                    std::lock_guard<std::mutex> lock(m_dataMutex);
                    m_data.clear();
                }

                std::size_t size() const
                {
                    std::lock_guard<std::mutex> lock(m_dataMutex);
                    const std::size_t s = m_data.size();

                    return s;
                }

                bool empty() const
                {
                    std::lock_guard<std::mutex> lock(m_dataMutex);
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
                mutable std::mutex m_dataMutex;
                std::chrono::time_point<std::chrono::steady_clock> m_last_access_time;

                Producer(TS_MultiHeadQueue<T>* queue): m_queue(queue), m_data(), m_dataMutex(), m_last_access_time()
                {
                    update_time();
                }

                T pop()
                {
                    std::lock_guard<std::mutex> lock(m_dataMutex);

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


        TS_MultiHeadQueue():
            m_producers(),
            m_producersMutex(),
            m_non_empty(),
            m_non_empty_mutex(),
            m_is_not_empty(),
            m_stopped(false)
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

        std::optional<T> pop()
        {
            // lock non empty producers
            std::unique_lock<std::mutex> lock(m_non_empty_mutex);
            wait_for_data(lock);

            return internal_pop();
        }

        std::optional<T> pop_for(const std::chrono::milliseconds& timeout)
        {
            std::optional<T> result;

            // lock non empty producers
            std::unique_lock<std::mutex> lock(m_non_empty_mutex);
            const bool status = wait_for_data(lock, timeout);

            if (status)
                result = std::move(internal_pop());

            return result;
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

        std::deque<Producer *> m_non_empty;
        std::mutex m_non_empty_mutex;

        std::condition_variable m_is_not_empty;

        std::atomic<bool> m_stopped;

        void release(Producer* producer)
        {
            // Remove Producer from non-empty producers
            // this ensures Producer will disaapear from both: m_non_empty and m_producers.
            // Without this one thread may remove producer while another will access it via m_non_empty.
            aboutToBeCleaned(producer);

            std::lock_guard<std::mutex> lock(m_producersMutex);
            m_producers.erase(producer);
        }

        void not_empty(Producer* producer)
        {
            std::lock_guard<std::mutex> lock(m_non_empty_mutex);

            auto f_it = std::find(m_non_empty.begin(), m_non_empty.end(), producer);
            if (f_it == m_non_empty.end())
                m_non_empty.push_back(producer);

            sort_non_empty_producers();

            m_is_not_empty.notify_one();
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

        std::optional<T> internal_pop()
        {
            std::optional<T> result;

            if (m_non_empty.empty() == false)
            {
                Producer* top = m_non_empty.front();
                result = std::move( top->pop() );

                if (top->empty())
                    m_non_empty.pop_front();

                sort_non_empty_producers();
            }

            return std::move(result);
        }

        void sort_non_empty_producers()
        {
            ProducerSorter sorter;
            std::sort(m_non_empty.begin(), m_non_empty.end(), sorter);
        }

        void aboutToBeCleaned(Producer* p)
        {
            std::lock_guard<std::mutex> lock(m_non_empty_mutex);

            for(typename std::deque<Producer *>::iterator it = m_non_empty.begin(); it != m_non_empty.end(); ++it)
                if (*it == p)
                {
                    m_non_empty.erase(it);
                    break;
                }
        }
};

#endif // TS_MULTIHEADQUEUE_H
