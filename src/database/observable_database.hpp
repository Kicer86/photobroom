
#ifndef OBSERVABLE_DATABASE_HPP_INCLUDED
#define OBSERVABLE_DATABASE_HPP_INCLUDED

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
            return typeid(T).name();
        }

    private:
        class Task: public Database::IDatabaseThread::ITask
        {
            public:
                Task(ObservableDatabase& executor, std::unique_ptr<Database::IDatabaseThread::ITask>&& task)
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
                std::unique_ptr<Database::IDatabaseThread::ITask> m_task;
                ObservableDatabase& m_executor;
        };

        friend class Task;

        void execute(std::unique_ptr<Database::IDatabaseThread::ITask>&& task) override
        {
            auto observedTask = std::make_unique<Task>(*this, std::move(task));

            T::execute(std::move(observedTask));
        }
};

#endif
