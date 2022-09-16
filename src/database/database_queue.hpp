
#ifndef DATABASE_QUEUE_HPP_INCLUDED
#define DATABASE_QUEUE_HPP_INCLUDED

#include <core/accumulative_queue.hpp>
#include <core/task_executor_utils.hpp>
#include <core/ts_resource.hpp>
#include <idatabase.hpp>


namespace Database
{
    /**
     * @brief Accumulative queue for database tasks
     *
     * DatabaseQueue collects tasks and passes them to db as one when
     * enough tasks were collected
     */
    class DatabaseQueue
    {
    public:
        DatabaseQueue(Database::IDatabase &);
        ~DatabaseQueue();

        void flush();
        void push(std::function<void(Database::IBackend &)>);

    private:
        using Queue = AccumulativeQueue<std::function<void(Database::IBackend &)>>;
        void flushQueue(Queue::Container &&);

        ol::ThreadSafeResource<Queue> m_queue;
        Database::IDatabase& m_db;
    };
}

#endif
