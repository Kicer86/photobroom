
#ifndef DATABASE_QUEUE_HPP_INCLUDED
#define DATABASE_QUEUE_HPP_INCLUDED

#include <core/task_executor_utils.hpp>
#include <idatabase.hpp>


namespace Database
{
    /**
     * @brief Queue for database tasks
     *
     * This is a twin brother of @ref TasksQueue
     *
     * @ref Database::IDatabase interface does not allow to remove
     * any task which was inserted (similarly to @ref TaskExecutor).
     *
     * Purpose of this class is to fix this issue by providing
     * a user controlled queue but for database tasks.
     */
    class DatabaseQueue: Queue<std::function<void(Database::IBackend &)>>
    {
    public:
        DatabaseQueue(Database::IDatabase &);
        ~DatabaseQueue() = default;

    private:
        void passTaskToExecutor(std::function<void(Database::IBackend &)>&& task, const Notifier &) override;

        Database::IDatabase& m_db;
    };
}

#endif
