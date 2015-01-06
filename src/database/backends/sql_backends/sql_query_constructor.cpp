
#include "sql_query_constructor.hpp"

namespace Database
{

    SqlQuery::SqlQuery(): m_queries()
    {

    }


    SqlQuery::SqlQuery(const QString& query): m_queries()
    {
        addQuery(query);
    }


    SqlQuery::~SqlQuery()
    {

    }


    void SqlQuery::addQuery(const QString& query)
    {
        m_queries.push_back(query);
    }


    const std::vector< QString >& SqlQuery::getQueries() const
    {
        return m_queries;
    }


    ISqlQueryConstructor::~ISqlQueryConstructor()
    {

    }

}
