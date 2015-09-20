
#ifndef ISQLQUERYEXECUTOR_HPP
#define ISQLQUERYEXECUTOR_HPP


#include "database/database_status.hpp"


class QString;
class QSqlQuery;


namespace Database
{
    class SqlMultiQuery;

    struct ISqlQueryExecutor
    {
        virtual ~ISqlQueryExecutor() {}

        virtual BackendStatus exec(const QString& query, QSqlQuery* result) const = 0;
        virtual BackendStatus exec(const SqlMultiQuery& query, QSqlQuery* result) const = 0;
    };
}


#endif // ISQLQUERYEXECUTOR_HPP
