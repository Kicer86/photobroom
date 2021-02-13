
#include <gmock/gmock.h>

#include "ibackend.hpp"

using namespace Database;

TEST(DbErrorTest, constructor)
{
    db_error err1("some text", StatusCodes::MigrationFailed, "details of error");
    db_error err2("some text", StatusCodes::OpenFailed, "details of error");

    EXPECT_STREQ(err1.what(), "some text: MigrationFailed, details of error");
    EXPECT_STREQ(err2.what(), "some text: OpenFailed, details of error");
}


TEST(DbErrorTest, status)
{
    db_error err1("some text", StatusCodes::MigrationFailed, "details of error");
    db_error err2("some text", StatusCodes::OpenFailed, "details of error");

    EXPECT_EQ(err1.status(), StatusCodes::MigrationFailed);
    EXPECT_EQ(err2.status(), StatusCodes::OpenFailed);
}
