
#ifndef DATABASE_BACKEND_HPP
#define DATABASE_BACKEND_HPP

#include "idatabase.hpp"

#include <utils/data_ptr.hpp>

#include "database_mysql_backend_export.h"


namespace Database
{

    class DATABASE_MYSQL_BACKEND_EXPORT MySqlBackend: public IBackend
    {
        public:
            MySqlBackend();
            virtual ~MySqlBackend();

            virtual bool store(const Entry &) override;
            virtual bool init() override;

        private:
            struct Data;
            data_ptr<Data> m_data;
    };

}

#endif
