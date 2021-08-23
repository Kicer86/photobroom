
#ifndef OBSERVABLE_DATABASE_HPP_INCLUDED
#define OBSERVABLE_DATABASE_HPP_INCLUDED

#include <core/observable_executor.hpp>
#include <database/idatabase.hpp>


template<typename T> requires std::is_base_of<Database::IDatabase, T>::value
class ObservableDatabase: public ObservableExecutor, public T
{
    public:
        template<typename ...Args>
        ObservableDatabase(Args... args): T(std::forward<Args>(args)...) {}

        QString name() const override
        {
            return typeid(T).name();
        }

    private:

};

#endif
