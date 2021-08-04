
#ifndef ACCUMULATIVE_QUEUE_HPP_INCLUDED
#define ACCUMULATIVE_QUEUE_HPP_INCLUDED

#include <chrono>
#include <deque>
#include <QObject>
#include <QTimer>


template<typename T>
class AccumulativeQueue: public QObject
{
    public:
        using Container = std::deque<T>;
        using ContainerIt = typename Container::iterator;

        AccumulativeQueue(std::size_t package_size_limit,
                          std::chrono::milliseconds max_waiting_time,
                          std::function<void(ContainerIt, ContainerIt)> flush)
            : m_flush(flush)
            , m_timeout(max_waiting_time)
            , m_sizeLimit(package_size_limit)
        {
            m_timer.setSingleShot(true);
            m_timer.callOnTimeout(this, &AccumulativeQueue::flush);
        };

        void push(const T& i)
        {
            m_queue.push_back(i);

            ensureTimerRunning();
            checkForFlush();
        }

    private:
        Container m_queue;
        QTimer m_timer;
        std::chrono::milliseconds m_timeout;
        std::function<void(ContainerIt, ContainerIt)> m_flush;
        std::size_t m_sizeLimit;

        void checkForFlush()
        {
            if (m_queue.size() >= m_sizeLimit)
            {
                m_timer.stop();
                flush();
            }
        }

        void ensureTimerRunning()
        {
            if (m_timer.isActive() == false)
                m_timer.start(m_timeout);
        }

        void flush()
        {
            m_flush(m_queue.begin(), m_queue.end());
            m_queue.clear();
        }
};

#endif // ACCUMULATIVE_QUEUE_HPP_INCLUDED
