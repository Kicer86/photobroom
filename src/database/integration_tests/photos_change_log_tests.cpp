
#include <gmock/gmock.h>

#include "common.hpp"

#include <core/base_tags.hpp>


// TODO: reenable


struct PhotosChangeLog: Tests::DatabaseTest
{

};


TEST_F(PhotosChangeLog, personIntroduction)
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

            // add tag
            Tag::TagsList tags;
            tags[TagNameInfo(BaseTagsList::Event)] = TagValue("test event");
            tags[TagNameInfo(BaseTagsList::Place)] = TagValue("test place");

            data_delta.insert<Photo::Field::Tags>(tags);
            op->update(data_delta);

            //expect "tag added" entry in change log
            QStringList changeLog = op->photoChangeLogOperator()->dumpChangeLog();

            changeLog.sort();

            ASSERT_EQ(changeLog.size(), 2);
            EXPECT_EQ(changeLog.front(), "photo id: 1. Tag added. Event: test event");
            EXPECT_EQ(changeLog.back(),  "photo id: 1. Tag added. Place: test place");
        });
    });
}

