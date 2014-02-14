
#ifndef DATABASE_BACKEND_HPP
#define DATABASE_BACKEND_HPP

#include "idatabase.hpp"

#include <memory>

#include "database_mysql_backend_export.h"
#include "../reusable/sql_backend.hpp"

namespace Database
{

    class DATABASE_MYSQL_BACKEND_EXPORT MySqlBackend final: public ASqlBackend
    {
        public:
            MySqlBackend();
            virtual ~MySqlBackend();

            virtual bool prepareDB(QSqlDatabase*) override;
            virtual QString addTag(const QString&) override;
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;
    };

}

#endif
