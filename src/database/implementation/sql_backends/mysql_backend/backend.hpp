
#ifndef DATABASE_MYSQL_BACKEND_HPP
#define DATABASE_MYSQL_BACKEND_HPP


#include <memory>

#include <database/idatabase.hpp>
#include <database/implementation/sql_backends/sql_backend.hpp>

#include "database_mysql_backend_export.h"

namespace Database
{

    class DATABASE_MYSQL_BACKEND_EXPORT MySqlBackend final: public ASqlBackend
    {
        public:
            MySqlBackend();
            virtual ~MySqlBackend();

            virtual bool prepareDB(QSqlDatabase*) override;
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;
    };

}

#endif
