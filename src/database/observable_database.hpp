
#ifndef OBSERVABLE_DATABASE_HPP_INCLUDED
#define OBSERVABLE_DATABASE_HPP_INCLUDED

#include <boost/type_index.hpp>

#include <core/observable_executor.hpp>
#include <database/idatabase.hpp>


template<typename T> requires std::is_base_of<Database::IDatabase, T>::value
class ObservableDatabase: public ObservableExecutor, public T
{
    public:
        template<typename ...Args>
        explicit ObservableDatabase(Args... args): T(std::forward<Args>(args)...) {}

        QString name() const override
        {
            return boost::typeindex::type_id<T>().pretty_name().c_str();
        }

    private:
        class Task: public Database::IDatabase::ITask
        {
            public:
                Task(ObservableDatabase& executor, std::unique_ptr<Database::IDatabase::ITask>&& task)
                    : m_task(std::move(task))
                    , m_executor(executor)
                {
                    m_executor.newTaskInQueue();
                }

                void run(Database::IBackend& backend) override
                {
                    m_executor.taskMovedToExecution();
                    m_task->run(backend);
                    m_executor.taskExecuted();
                }

                std::string name() override
                {
                    return m_task->name();
                }

            private:
                std::unique_ptr<Database::IDatabase::ITask> m_task;
                ObservableDatabase& m_executor;
        };

        friend class Task;

        void execute(std::unique_ptr<Database::IDatabase::ITask>&& task) override
        {
            auto observedTask = std::make_unique<Task>(*this, std::move(task));

            T::execute(std::move(observedTask));
        }
};

#endif
