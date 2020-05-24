
#include "database_tools/json_to_backend.hpp"

#include "common.hpp"
#include "sample_db.hpp"


struct PhotoOperatorTest: Tests::DatabaseTest
{
    PhotoOperatorTest(): Tests::DatabaseTest()
    {

    }

    ~PhotoOperatorTest()
    {

    }
};


TEST_F(PhotoOperatorTest, gettingAllPhotos)
{
    for_all_db_plugins([](Database::IBackend* op)
    {
        Database::JsonToBackend converter(*op);
        converter.append(SampleDB::db1);
    });
}
