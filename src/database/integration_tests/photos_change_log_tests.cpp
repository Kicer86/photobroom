
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

