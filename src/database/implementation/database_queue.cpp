
#include "database_queue.hpp"

namespace Database
{

    DatabaseQueue::DatabaseQueue(Database::IDatabase& db)
        : Queue(1, Mode::Fifo)
        , m_db(db)
    {

    }


    void DatabaseQueue::passTaskToExecutor(std::function<void(Database::IBackend &)>&& task, const Notifier& notifier)
    {
        m_db.exec([task, notifier](Database::IBackend& backend)
        {
            task(backend);
        });
    }

}
