
#ifndef DATABASE_SQLITE_BACKEND_HPP
#define DATABASE_SQLITE_BACKEND_HPP

#include <memory>

#include <database/idatabase.hpp>
#include <database/backends/sql_backends/sql_backend.hpp>
#include <database/backends/sql_backends/generic_sql_query_constructor.hpp>
#include "database_sqlite_backend_export.h"

namespace Database
{

    class DATABASE_SQLITE_BACKEND_EXPORT SQLiteBackend final: public ASqlBackend, GenericSqlQueryConstructor
    {
        public:
            SQLiteBackend(IConfiguration *, ILogger *);
            virtual ~SQLiteBackend();

        private:
            // ASqlBackend:
            virtual BackendStatus prepareDB(const ProjectInfo &) override;
            virtual bool dbOpened() override;
            virtual const IGenericSqlQueryGenerator& getGenericQueryGenerator() const override;

            //ISqlQueryConstructor:
            virtual QString prepareFindTableQuery(const QString &) const override;
            virtual QString getTypeFor(ColDefinition::Purpose) const override;

            struct Data;
            std::unique_ptr<Data> m_data;
    };
}

#endif
