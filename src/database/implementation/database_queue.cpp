
#include "database_queue.hpp"

using namespace std::placeholders;

namespace Database
{

    DatabaseQueue::DatabaseQueue(Database::IDatabase& db)
        : m_queue(1000u, std::bind(&DatabaseQueue::flushQueue, this, _1))
        , m_db(db)
    {

    }


    DatabaseQueue::~DatabaseQueue()
    {
        flush();
    }


    void DatabaseQueue::flush()
    {
        m_queue.lock()->flush();
    }


    void DatabaseQueue::pushPackibleTask(std::function<void(Database::IBackend &)>&& task)
    {
        m_queue.lock()->push(std::move(task));
    }


    void DatabaseQueue::flushQueue(Queue::Container&& tasks)
    {
        m_db.exec([dbTasks = std::move(tasks)](Database::IBackend& backend)
        {
            auto _ = backend.openTransaction();

            for (auto& task: dbTasks)
                task(backend);
        });
    }

}
