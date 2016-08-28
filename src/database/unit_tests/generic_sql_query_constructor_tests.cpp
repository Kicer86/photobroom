
#include <gtest/gtest.h>

#include "generic_sql_query_generator.hpp"


// Database::GenericSqlQueryConstructor is abstract class,
// add missing implementations so it can be tested.

struct GenericSqlQueryConstructorImpl: public Database::GenericSqlQueryConstructor
{
    virtual QString getTypeFor(Database::ColDefinition::Purpose) const
    {
        return "_ID_";
    }
};


TEST(GenericSqlQueryConstructorTest, isContructible)
{
    EXPECT_NO_THROW(
    {
        GenericSqlQueryConstructorImpl constructor;
    });
}

