
#ifndef DATABASE_IGENERIC_SQL_QUERY_GENERATOR
#define DATABASE_IGENERIC_SQL_QUERY_GENERATOR

#include <vector>

#include <QString>

#include "sql_backend_base_export.h"
#include "table_definition.hpp"

namespace Database
{

    class InsertQueryData;
    class UpdateQueryData;

    struct ColDefinition;

    struct IGenericSqlQueryGenerator
    {
        virtual ~IGenericSqlQueryGenerator() {}

        // Create table with given name and columns decription.
        // It may be necessary for table to meet features:
        // - FOREIGN KEY
        //
        // More features may be added in future.
        // Default implementation returns QString("CREATE TABLE %1(%2);").arg(name).arg(columnsDesc)
        virtual QString prepareCreationQuery(const QString& name, const QString& columns) const = 0;

        //prepare query for finding table with given name
        virtual QString prepareFindTableQuery(const QString& name) const = 0;

        // get type for column's purpose
        virtual QString getTypeFor(ColDefinition::Purpose) const = 0;

        virtual std::vector<QString> insert(const InsertQueryData &) const = 0;             // construct an insert sql query.
        virtual std::vector<QString> update(const UpdateQueryData &) const = 0;             // construct an update sql query.
    };
}

#endif
