
#include <gtest/gtest.h>

#include "unit_tests_utils/printers.hpp"

#include "generic_sql_query_constructor.hpp"
#include "query_structs.hpp"


// Database::GenericSqlQueryConstructor is abstract class,
// add missing implementations so it can be tested.

struct GenericSqlQueryConstructorImpl: public Database::GenericSqlQueryConstructor
{
    GenericSqlQueryConstructorImpl() {}

    virtual QString getTypeFor(Database::ColDefinition::Purpose) const
    {
        return "_ID_";
    }
};


TEST(GenericSqlQueryConstructorTest, isContructible)
{
    EXPECT_NO_THROW(
    {
        const GenericSqlQueryConstructorImpl constructor;
    });
}


TEST(GenericSqlQueryConstructorTest, simpleInsertQuery)
{
    const GenericSqlQueryConstructorImpl constructor;

    Database::InsertQueryData data("table");
    data.setColumns("id", "name");
    data.setValues("id_value", "name_value");
    const QString queryStr = constructor.prepareInsertQuery(data);

    EXPECT_EQ(queryStr, "INSERT INTO table(id, name) VALUES(:id, :name)");
}


TEST(GenericSqlQueryConstructorTest, simpleUpdateQuery)
{
    const GenericSqlQueryConstructorImpl constructor;

    Database::UpdateQueryData data("table");
    data.setColumns("id", "name");
    data.setValues("id_value", "name_value");
    data.setCondition("pkg", "pkg_value");
    const QString queryStr = constructor.prepareUpdateQuery(data);

    EXPECT_EQ(queryStr, "UPDATE table SET id=:id, name=:name WHERE pkg=:pkg");
}


TEST(GenericSqlQueryConstructorTest, timestampInsertQuery)
{
    const GenericSqlQueryConstructorImpl constructor;

    Database::InsertQueryData data("table");
    data.setColumns("id", "time");
    data.setValues("id_value", Database::UpdateQueryData::Value::CurrentTime);
    const QString queryStr = constructor.prepareInsertQuery(data);

    EXPECT_EQ(queryStr, "INSERT INTO table(id, time) VALUES(:id, CURRENT_TIMESTAMP)");
}
