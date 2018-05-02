
#ifndef ISQLQUERYEXECUTOR_HPP
#define ISQLQUERYEXECUTOR_HPP

#include <vector>

#include "database/database_status.hpp"


class QString;
class QSqlQuery;


namespace Database
{

    struct ISqlQueryExecutor
    {
        virtual ~ISqlQueryExecutor() {}

        virtual BackendStatus prepare(const QString& query, QSqlQuery* result) const = 0;
        virtual BackendStatus exec(const QString& query, QSqlQuery* result) const = 0;
        virtual BackendStatus exec(const std::vector<QString>& query, QSqlQuery* result) const = 0;
        virtual BackendStatus exec(QSqlQuery& query) const = 0;
    };

}


#endif // ISQLQUERYEXECUTOR_HPP
