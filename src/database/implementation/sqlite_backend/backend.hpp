
#ifndef DATABASE_SQLITE_BACKEND_HPP
#define DATABASE_SQLITE_BACKEND_HPP

#include "idatabase.hpp"

#include <memory>

#include "database_sqlite_backend_export.h"
#include "../reusable/sql_backend.hpp"

namespace Database
{

    class DATABASE_SQLITE_BACKEND_EXPORT SQLiteBackend final: public ASqlBackend
    {
        public:
            SQLiteBackend();
            virtual ~SQLiteBackend();

            virtual bool prepareDB(QSqlDatabase*) override;
            virtual QString prepareCreationQuery(const QString& name, const QString& columns) const override;

        private:
            struct Data;
            std::unique_ptr<Data> m_data;
    };

}

#endif
