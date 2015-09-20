
#include "sql_query_constructor.hpp"

namespace Database
{

    SqlMultiQuery::SqlMultiQuery(): m_queries()
    {

    }


    SqlMultiQuery::SqlMultiQuery(const QString& query): m_queries()
    {
        addQuery(query);
    }


    SqlMultiQuery::SqlMultiQuery(const std::initializer_list<QString>& l): m_queries(l)
    {
        
    }


    SqlMultiQuery::~SqlMultiQuery()
    {

    }


    void SqlMultiQuery::addQuery(const QString& query)
    {
        m_queries.push_back(query);
    }


    const std::vector< QString >& SqlMultiQuery::getQueries() const
    {
        return m_queries;
    }


    ISqlQueryConstructor::~ISqlQueryConstructor()
    {

    }

}
