
#include <gmock/gmock.h>

#include "common.hpp"

#include <core/base_tags.hpp>


// TODO: reenable


struct PhotosChangeLog: Tests::DatabaseTest
{

};


TEST_F(PhotosChangeLog, tagsManipulation)
{
    for_all_db_plugins([](Database::IDatabase* db)
    {
        db->exec([](Database::IBackend* op)
        {
            // add new photo
            Photo::DataDelta pd;
            pd.insert<Photo::Field::Path>("photo1.jpeg");

            std::vector<Photo::DataDelta> photos = { pd };
            ASSERT_TRUE(op->addPhotos(photos));

            // read photo structure
            Photo::DataDelta data_delta(photos.front().getId());

            // add tags
            Tag::TagsList tags;
            tags[TagNameInfo(BaseTagsList::Event)] = TagValue("test event");

            data_delta.insert<Photo::Field::Tags>(tags);
            op->update(data_delta);

            tags[TagNameInfo(BaseTagsList::Place)] = TagValue("test place");

            data_delta.insert<Photo::Field::Tags>(tags);
            op->update(data_delta);

            // modify tag
            tags[TagNameInfo(BaseTagsList::Event)] = TagValue("test event 2");

            data_delta.insert<Photo::Field::Tags>(tags);
            op->update(data_delta);

            // remove tag
            tags.erase(TagNameInfo(BaseTagsList::Place));

            data_delta.insert<Photo::Field::Tags>(tags);
            op->update(data_delta);

            // verify change log
            const QStringList changeLog = op->photoChangeLogOperator()->dumpChangeLog();

            ASSERT_EQ(changeLog.size(), 4);
            EXPECT_EQ(changeLog[0], "photo id: 1. Tag added. Event: test event");
            EXPECT_EQ(changeLog[1], "photo id: 1. Tag added. Place: test place");
            EXPECT_EQ(changeLog[2], "photo id: 1. Tag modified. Event: test event -> test event 2");
            EXPECT_EQ(changeLog[3], "photo id: 1. Tag removed. Place: test place");
        });
    });
}


TEST_F(PhotosChangeLog, groupsManipulation)
{
    for_all_db_plugins([](Database::IDatabase* db)
    {
        db->exec([](Database::IBackend* op)
        {
            // add some photos
            Photo::DataDelta pd0,pd1,pd2,pd3,pd4,pd5;

            pd0.insert<Photo::Field::Path>("photo0.jpeg");
            pd1.insert<Photo::Field::Path>("photo1.jpeg");
            pd2.insert<Photo::Field::Path>("photo2.jpeg");
            pd3.insert<Photo::Field::Path>("photo1.jpeg");
            pd4.insert<Photo::Field::Path>("photo4.jpeg");
            pd5.insert<Photo::Field::Path>("photo5.jpeg");

            std::vector<Photo::DataDelta> photos = { pd0,pd1,pd2,pd3,pd4,pd5 };
            ASSERT_TRUE(op->addPhotos(photos));

            // create groups
            const Group::Id gr1 = op->groupOperator()->addGroup(photos[0].getId(), Group::Animation);
            const Group::Id gr2 = op->groupOperator()->addGroup(photos[3].getId(), Group::Animation);

            // read photo structure
            Photo::DataDelta data_delta1(photos[1].getId());
            Photo::DataDelta data_delta2(photos[2].getId());

            // add to group
            const GroupInfo gr_info1 = { .group_id = gr1, .role = GroupInfo::Member };
            data_delta1.insert<Photo::Field::GroupInfo>(gr_info1);
            op->update(data_delta1);

            const GroupInfo gr_info2 = { .group_id = gr1, .role = GroupInfo::Member };
            data_delta2.insert<Photo::Field::GroupInfo>(gr_info2);
            op->update(data_delta2);

            // verify change log
            const QStringList changeLog = op->photoChangeLogOperator()->dumpChangeLog();

            ASSERT_EQ(changeLog.size(), 2);
            EXPECT_EQ(changeLog[0], "photo id: 2. Group added. 1: 2");
            EXPECT_EQ(changeLog[1], "photo id: 3. Group added. 1: 2");

        });
    });
}
