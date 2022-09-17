
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
        explicit DatabaseQueue(Database::IDatabase &);
        ~DatabaseQueue();

        void flush();

        template<typename C>
        void pushTask(C&& task)                                                ///< task will be send directly to db
        {
            m_db.exec(std::move(task));
        }

        void pushPackibleTask(std::function<void(Database::IBackend &)> &&);   ///< task will be send to db as a pack with other tasks

    private:
        using Queue = AccumulativeQueue<std::function<void(Database::IBackend &)>>;
        void flushQueue(Queue::Container &&);

        ol::ThreadSafeResource<Queue> m_queue;
        Database::IDatabase& m_db;
    };
}


template<typename T>
struct ExecutorTraits<Database::DatabaseQueue, T>
{
    static void exec(Database::DatabaseQueue& queue, T&& t)
    {
        queue.pushTask(std::move(t));
    }
};


#endif
