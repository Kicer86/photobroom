
#include "database_tools/json_to_backend.hpp"

#include "common.hpp"
#include "sample_db.json.hpp"

using testing::Contains;


MATCHER_P(IsPhotoWithPath, _path, "") {
    return arg.path == _path;
}


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
        // fill backend with sample data
        Database::JsonToBackend converter(*op);
        converter.append(SampleDB::db1);

        const auto photos = op->photoOperator().getPhotos({});

        ASSERT_EQ(photos.size(), 3);

        std::vector<Photo::Data> photo_data;
        photo_data.push_back(op->getPhoto(photos[0]));
        photo_data.push_back(op->getPhoto(photos[1]));
        photo_data.push_back(op->getPhoto(photos[2]));

        EXPECT_THAT(photo_data, Contains(IsPhotoWithPath("/some/path1.jpeg")));
        EXPECT_THAT(photo_data, Contains(IsPhotoWithPath("/some/path2.jpeg")));
        EXPECT_THAT(photo_data, Contains(IsPhotoWithPath("/some/path3.jpeg")));
    });
}
