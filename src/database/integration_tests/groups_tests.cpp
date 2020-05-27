
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
    for_all_db_plugins([](Database::IBackend* op)
    {
        // store 3 photos
        Photo::DataDelta pd1, pd2, pd3;
        pd1.insert<Photo::Field::Path>("photo1.jpeg");
        pd2.insert<Photo::Field::Path>("photo2.jpeg");
        pd3.insert<Photo::Field::Path>("photo3.jpeg");

        std::vector<Photo::DataDelta> photos = { pd1, pd2, pd3 };
        ASSERT_TRUE(op->addPhotos(photos));

        Photo::Id modified_photo;
        QObject::connect(op, &Database::IBackend::photoModified, [&modified_photo](const Photo::Id& id)
        {
            modified_photo = id;
        });

        // create group
        const Photo::Id& id1 = photos[0].getId();
        const Group::Id& gid = op->groupOperator().addGroup(id1, Group::Type::Animation);
        EXPECT_TRUE(gid.valid());

        // expect representative photo to be modified
        ASSERT_TRUE(modified_photo.valid());
        EXPECT_EQ(modified_photo, id1);
    });
}



TEST_F(GroupsTest, groupRemoval)
{
    for_all_db_plugins([](Database::IBackend* op)
    {
        // store 3 photos
        Photo::DataDelta pd1, pd2, pd3;
        pd1.insert<Photo::Field::Path>("photo1.jpeg");
        pd2.insert<Photo::Field::Path>("photo2.jpeg");
        pd3.insert<Photo::Field::Path>("photo3.jpeg");

        std::vector<Photo::DataDelta> photos = { pd1, pd2, pd3 };
        ASSERT_TRUE(op->addPhotos(photos));

        // create group
        const Photo::Id& id1 = photos[0].getId();
        const Group::Id& gid = op->groupOperator().addGroup(id1, Group::Type::Animation);
        EXPECT_TRUE(gid.valid());

        // add photos to group
        const GroupInfo grpInfo(gid, GroupInfo::Member);
        pd2.clear();
        pd3.clear();
        pd2.setId(photos[1].getId());
        pd3.setId(photos[2].getId());
        pd2.insert<Photo::Field::GroupInfo>(grpInfo);
        pd3.insert<Photo::Field::GroupInfo>(grpInfo);

        op->update(pd2);
        op->update(pd3);

        // watch for changes
        std::set<Photo::Id> modified_photos;
        QObject::connect(op, &Database::IBackend::photoModified, [&modified_photos](const Photo::Id& id)
        {
            modified_photos.insert(id);
        });

        // remove group
        op->groupOperator().removeGroup(gid);

        // expect all photos to be modified
        ASSERT_EQ(modified_photos.size(), 3);
    });
}
