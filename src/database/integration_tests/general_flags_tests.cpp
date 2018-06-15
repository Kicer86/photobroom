
#include <gmock/gmock.h>

#include "common.hpp"


struct GeneralFlagsTest: Tests::DatabaseTest
{
    GeneralFlagsTest(): Tests::DatabaseTest()
    {

    }

    ~GeneralFlagsTest()
    {

    }
};


TEST_F(GeneralFlagsTest, flagsIntroduction)
{
    for_all([](Database::IDatabase* db)
    {
        db->performCustomAction([](Database::IBackendOperator* op)
        {

        });
    });
}

