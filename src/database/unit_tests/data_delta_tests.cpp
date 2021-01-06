
#include <gmock/gmock.h>

#include "photo_data.hpp"

using testing::UnorderedElementsAre;


TEST(DataDeltaTest, merging)
{
    Photo::DataDelta d1(Photo::Id(1));
    Photo::DataDelta d2(Photo::Id(1));

    d1.insert<Photo::Field::Checksum>("1111");
    d1.insert<Photo::Field::Flags>( {{Photo::FlagsE::ExifLoaded, 1}} );

    d2.insert<Photo::Field::Geometry>(QSize(10, 20));
    d2.insert<Photo::Field::Path>("2222");
    d2.insert<Photo::Field::Flags>( {{Photo::FlagsE::Sha256Loaded, 1}} );

    d1 |= d2;

    ASSERT_TRUE(d1.has(Photo::Field::Checksum));
    ASSERT_TRUE(d1.has(Photo::Field::Geometry));
    ASSERT_TRUE(d1.has(Photo::Field::Path));
    ASSERT_TRUE(d1.has(Photo::Field::Flags));

    EXPECT_EQ(d1.get<Photo::Field::Checksum>(), "1111");
    EXPECT_EQ(d1.get<Photo::Field::Geometry>(), QSize(10, 20));
    EXPECT_EQ(d1.get<Photo::Field::Path>(), "2222");
    EXPECT_THAT(d1.get<Photo::Field::Flags>(), UnorderedElementsAre( std::pair{Photo::FlagsE::ExifLoaded, 1}, std::pair{Photo::FlagsE::Sha256Loaded, 1} ));
}
