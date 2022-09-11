
#include <gmock/gmock.h>

#include "photo_data.hpp"

using testing::UnorderedElementsAre;


TEST(DataDeltaTest, merging)
{
    Photo::DataDelta d1(Photo::Id(1));
    Photo::DataDelta d2(Photo::Id(1));

    d1.insert<Photo::Field::Flags>( {{Photo::FlagsE::ExifLoaded, 1}} );

    d2.insert<Photo::Field::Geometry>(QSize(10, 20));
    d2.insert<Photo::Field::Path>("2222");
    d2.insert<Photo::Field::Flags>( {{Photo::FlagsE::StagingArea, 1}} );

    d1 |= d2;

    ASSERT_TRUE(d1.has(Photo::Field::Geometry));
    ASSERT_TRUE(d1.has(Photo::Field::Path));
    ASSERT_TRUE(d1.has(Photo::Field::Flags));

    EXPECT_EQ(d1.get<Photo::Field::Geometry>(), QSize(10, 20));
    EXPECT_EQ(d1.get<Photo::Field::Path>(), "2222");
    EXPECT_THAT(d1.get<Photo::Field::Flags>(), UnorderedElementsAre( std::pair{Photo::FlagsE::ExifLoaded, 1}, std::pair{Photo::FlagsE::StagingArea, 1} ));
}


TEST(DataDeltaTest, mergingWithEmpty)
{
    Photo::DataDelta d1;
    Photo::DataDelta d2(Photo::Id(1));

    d2.insert<Photo::Field::Geometry>(QSize(10, 20));
    d2.insert<Photo::Field::Path>("2222");
    d2.insert<Photo::Field::Flags>( {{Photo::FlagsE::ExifLoaded, 1}} );

    d1 |= d2;

    EXPECT_EQ(d1.getId(), d2.getId());
}


TEST(DataDeltaTest, dataAndDataDeltaConversion)
{
    Photo::Data d1;
    d1.id = 15;
    d1.tags = { {Tag::Types::Place, QString("somewhere")} };
    d1.flags = { {Photo::FlagsE::StagingArea, 1} };
    d1.path = "/path/file.jpeg";
    d1.geometry = QSize(100,200);
    d1.groupInfo = GroupInfo(Group::Id(89), GroupInfo::Member);

    // Photo::Data to Photo::Delta
    const Photo::DataDelta d2(d1);

    // back to Photo::Data
    Photo::Data d3;
    d3.apply(d2);

    // original and recreated Datas should be equal
    EXPECT_EQ(d1, d3);
}


TEST(DataDeltaTest, DataDiff)
{
    Photo::Data oldData;
    oldData.id = Photo::Id(15);
    oldData.flags[Photo::FlagsE::ExifLoaded] = 1;
    oldData.flags[Photo::FlagsE::GeometryLoaded] = 2;
    oldData.geometry = QSize(100, 200);
    oldData.groupInfo = GroupInfo(Group::Id(5), GroupInfo::Member);
    oldData.path = "1234";
    oldData.tags[Tag::Types::Event] = QString("tttr");
    oldData.phash = Photo::PHashT(0x1234567890123456LL);

    Photo::Data newData1(oldData);
    newData1.flags[Photo::FlagsE::StagingArea] = 1;
    newData1.flags[Photo::FlagsE::ExifLoaded] = 0;
    newData1.geometry = QSize(200, 200);
    newData1.groupInfo = GroupInfo(Group::Id(6), GroupInfo::Member);
    newData1.path = "12345";
    newData1.tags[Tag::Types::Event] = QString("tttrq");
    oldData.phash = Photo::PHashT(0xabcdef0011223344LL);

    Photo::Data newData2(oldData);

    Photo::DataDelta d1(oldData, newData1);
    Photo::DataDelta d2(oldData, newData2);

    EXPECT_EQ(d1.getId(), oldData.id);
    EXPECT_THAT(d1.get<Photo::Field::Flags>(), UnorderedElementsAre( std::pair{Photo::FlagsE::GeometryLoaded, 2},
                                                                     std::pair{Photo::FlagsE::StagingArea, 1},
                                                                     std::pair{Photo::FlagsE::ExifLoaded, 0} ));
    EXPECT_EQ(d1.get<Photo::Field::Geometry>(), QSize(200, 200));
    EXPECT_EQ(d1.get<Photo::Field::GroupInfo>(), GroupInfo(Group::Id(6), GroupInfo::Member));
    EXPECT_EQ(d1.get<Photo::Field::Path>(), "12345");
    EXPECT_THAT(d1.get<Photo::Field::Tags>(), UnorderedElementsAre( std::pair{Tag::Types::Event, QString("tttrq")} ));
    EXPECT_EQ(d1.get<Photo::Field::PHash>(), newData1.phash);

    EXPECT_EQ(d2.getId(), oldData.id);
    EXPECT_FALSE(d2.has(Photo::Field::Flags));
    EXPECT_FALSE(d2.has(Photo::Field::Geometry));
    EXPECT_FALSE(d2.has(Photo::Field::GroupInfo));
    EXPECT_FALSE(d2.has(Photo::Field::Path));
    EXPECT_FALSE(d2.has(Photo::Field::Tags));
    EXPECT_FALSE(d2.has(Photo::Field::PHash));
}
