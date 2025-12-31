
#ifndef DATABASE_EXECUTOR_TRAITS_HPP_INCLUDED
#define DATABASE_EXECUTOR_TRAITS_HPP_INCLUDED

#include <core/task_executor_utils.hpp>
#include <database/idatabase.hpp>

template<typename T>
struct ExecutorTraits<Database::IDatabase, T>
{
    static void exec(Database::IDatabase& db, T&& t, std::string_view name)
    {
        db.exec(std::forward<T>(t), name);
    }
};


#endif
