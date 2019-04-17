
#include <gmock/gmock.h>

#include "common.hpp"


struct GroupsTest: Tests::DatabaseTest
{
    GroupsTest(): Tests::DatabaseTest()
    {

    }

    ~GroupsTest()
    {

    }
};


TEST_F(GroupsTest, groupCreation)
{
    for_all_db_plugins([](Database::IDatabase* db)
    {
        db->exec([](Database::IBackend* op)
        {
            // store 3 photos
            Photo::DataDelta pd1, pd2, pd3;
            pd1.insert<Photo::Field::Path>("photo1.jpeg");
            pd2.insert<Photo::Field::Path>("photo2.jpeg");
            pd3.insert<Photo::Field::Path>("photo3.jpeg");

            std::vector<Photo::Id> ids;
            std::vector<Photo::DataDelta> photos = { pd1, pd2, pd3 };
            ASSERT_TRUE(op->addPhotos(photos));

            Photo::Id modified_photo;
            QObject::connect(op, &Database::IBackend::photoModified, [&modified_photo](const Photo::Id& id)
            {
                modified_photo = id;
            });

            const Photo::Id& id1 = photos[0].getId();
            const Group::Id& gid = op->groupOperator()->addGroup(id1, GroupInfo::Type::Animation);

            EXPECT_TRUE(gid.valid());

            EXPECT_EQ(modified_photo, id1);
        });
    });
}


