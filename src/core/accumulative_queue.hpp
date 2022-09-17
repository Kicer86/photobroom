
#ifndef ACCUMULATIVE_QUEUE_HPP_INCLUDED
#define ACCUMULATIVE_QUEUE_HPP_INCLUDED

#include <deque>
#include <functional>


template<typename T>
class AccumulativeQueue
{
    public:
        using Container = std::deque<T>;

        AccumulativeQueue(std::size_t package_size_limit,
                          std::function<void(Container &&)> flush)
            : m_flush(flush)
            , m_sizeLimit(package_size_limit)
        {

        };

        void push(T&& i)
        {
            m_queue.push_back(std::move(i));

            checkForFlush();
        }

        void flush()
        {
            Container package;
            package.swap(m_queue);

            m_flush(std::move(package));
        }

    private:
        Container m_queue;
        std::function<void(Container &&)> m_flush;
        std::size_t m_sizeLimit;

        void checkForFlush()
        {
            if (m_queue.size() >= m_sizeLimit)
                flush();
        }
};

#endif // ACCUMULATIVE_QUEUE_HPP_INCLUDED
