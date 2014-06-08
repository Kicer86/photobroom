
#ifndef DATABASE_ISQL_QUERY_CONSTRUCTOR
#define DATABASE_ISQL_QUERY_CONSTRUCTOR

#include <vector>

#include <QString>

namespace Database
{

    class InsertQueryData;
    class UpdateQueryData;

    struct SqlQuery
    {
        SqlQuery();
        SqlQuery(const QString &);
        ~SqlQuery();

        void addQuery(const QString &);

        const std::vector<QString>& getQueries() const;

        private:
            std::vector<QString> m_queries;
    };


    struct ISqlQueryConstructor
    {
        virtual ~ISqlQueryConstructor();

        virtual SqlQuery insert(const InsertQueryData &) = 0;                 // construct an insert sql query.
        virtual SqlQuery update(const UpdateQueryData &) = 0;                 // construct an update sql query.
        virtual SqlQuery insertOrUpdate(const InsertQueryData &) = 0;         // construct a query which will try to insert data. If it fails due to UNIQUE column attribute, try to update
    };
}

#endif

