
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
    for_all_db_plugins([](Database::IDatabase* db)
    {
        db->performCustomAction([](Database::IBackendOperator* op)
        {
            // store 2 photos
            Photo::DataDelta pd1, pd2;
            pd1.insert<Photo::Field::Path>("photo1.jpeg");
            pd2.insert<Photo::Field::Path>("photo2.jpeg");

            std::vector<Photo::Id> ids;
            std::vector<Photo::DataDelta> photos = { pd1, pd2 };
            op->addPhotos(photos);

            ids.push_back(photos.front().getId());
            ids.push_back(photos.back().getId());

            op->set(ids[0], "test1", 1);
            op->set(ids[0], "test2", 2);
            op->set(ids[1], "test3", 3);
            op->set(ids[1], "test4", 4);

            EXPECT_EQ(op->get(ids[0], "test2"), 2);
            EXPECT_EQ(op->get(ids[0], "test1"), 1);
            EXPECT_EQ(op->get(ids[1], "test4"), 4);
            EXPECT_EQ(op->get(ids[1], "test3"), 3);
        });
    });
}


TEST_F(GeneralFlagsTest, invalidName)
{
    for_all_db_plugins([](Database::IDatabase* db)
    {
        db->performCustomAction([](Database::IBackendOperator* op)
        {
            // store 2 photos
            Photo::DataDelta pd1;
            pd1.insert<Photo::Field::Path>("photo1.jpeg");

            std::vector<Photo::Id> ids;
            std::vector<Photo::DataDelta> photos = { pd1 };
            op->addPhotos(photos);

            ids.push_back(photos.front().getId());

            EXPECT_FALSE(op->get(ids[0], "test2").has_value());
            EXPECT_FALSE(op->get(ids[0], "test1").has_value());
        });
    });
}
