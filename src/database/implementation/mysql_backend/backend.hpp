
#ifndef DATABASE_BACKEND_HPP
#define DATABASE_BACKEND_HPP

#include "idatabase.hpp"

#include <memory>

#include "database_mysql_backend_export.h"
#include "../sql_backend.hpp"

namespace Database
{

    class DATABASE_MYSQL_BACKEND_EXPORT MySqlBackend: public ASqlBackend
    {
        public:
            MySqlBackend();
            virtual ~MySqlBackend();

            virtual bool openDB(QSqlDatabase*) override;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;
    };

}

#endif
