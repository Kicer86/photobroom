
#ifndef DATABASE_ISQL_QUERY_CONSTRUCTOR
#define DATABASE_ISQL_QUERY_CONSTRUCTOR

#include <vector>

#include <QString>

class InsertQueryData;
class UpdateQueryData;

namespace Database
{

    struct SqlQuery
    {
        SqlQuery();
        ~SqlQuery();

        void addQuery(const QString &);

        const std::vector<QString>& getQueries() const;

        private:
            std::vector<QString> m_queries;
    };


    struct ISqlQueryConstructor
    {
        virtual ~ISqlQueryConstructor();

        virtual SqlQuery insert(const InsertQueryData &) = 0;
        virtual SqlQuery update(const UpdateQueryData &) = 0;
        virtual SqlQuery insertOrUpdate(const InsertQueryData &) = 0;
    };
}

#endif

