
#include <iostream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "photo_iterator.hpp"

namespace
{
    struct MockQuery: Database::IQuery
    {
        MOCK_METHOD0(gotoNext, bool());
        MOCK_CONST_METHOD0(getId, PhotoInfo::Id());
        MOCK_CONST_METHOD0(valid, bool());
        MOCK_CONST_METHOD0(size, int());
        MOCK_CONST_METHOD0(backend, Database::IBackend*());
        MOCK_CONST_METHOD0(clone, Database::IQuery*());
        MOCK_METHOD0(clone, std::shared_ptr<IQuery>());
    };
}


TEST(PhotoIteratorShould, BeInvalidWhenConstructedWithNoArguments)
{
    Database::PhotoIterator photoIt;

    ASSERT_EQ(false, static_cast<bool>(photoIt));
    ASSERT_EQ(true, ! photoIt);
}

//TODO: fix
TEST(PhotoIteratorShould, beAbleToMoveToNextPhotoBasingOnPhotoId)
{
    Database::InterfaceContainer<MockQuery> queryContainer(new MockQuery);

    using ::testing::Return;
    using ::testing::AtLeast;

    //initial condition in ++operator
    EXPECT_CALL(*static_cast<MockQuery *>(*queryContainer), valid()).Times(AtLeast(1)).WillOnce(Return(true));

    //first in constructor, when twice in ++operator until we move to second photo (from first row to second, and from second to third)
    EXPECT_CALL(*static_cast<MockQuery *>(*queryContainer), gotoNext()).Times(3).WillRepeatedly(Return(true));

    //called three times on each of rows
    EXPECT_CALL(*static_cast<MockQuery *>(*queryContainer), getId())
        .Times(3)
        .WillOnce(Return(PhotoInfo::Id(1)))
        .WillOnce(Return(PhotoInfo::Id(1)))
        .WillOnce(Return(PhotoInfo::Id(2)));

    //Database::PhotoIterator photoIt(queryContainer);

    //++photoIt;
}


//TODO: fix
TEST(PhotoIteratorShould, moveToFirstRowOfDataWhenConstructedWithQuery)
{
    auto query = std::make_shared<MockQuery>();

    using ::testing::Return;
    EXPECT_CALL(*query, gotoNext()).Times(1).WillOnce(Return(true));

    //Database::PhotoIterator photoIt(query);
}

/*
TEST(PhotoIteratorShould, BeCopyable)
{
    //TODO: state machine required?
    auto query = std::make_shared<MockQuery>();

    using ::testing::Return;
    using ::testing::AtLeast;
    using ::testing::_;

    EXPECT_CALL(*query, valid()).Times(AtLeast(1)).WillOnce(Return(true));

    //first in constructor, when once in ++operator
    EXPECT_CALL(*query, gotoNext()).Times(2).WillRepeatedly(Return(true));

    //called two times on each of rows
    EXPECT_CALL(*query, getField(Database::IQuery::Fields::Id))
        .Times(2)
        .WillOnce(Return(QVariant("1")))
        .WillOnce(Return(QVariant("2")));


    EXPECT_CALL(*query, getField(Database::IQuery::Fields::Path))
        .Times(2)
        .WillOnce(Return(QVariant("p1")))
        .WillOnce(Return(QVariant("p2")));

    EXPECT_CALL(*query, getField(_));

    Database::PhotoIterator photoIt(query);
    Database::PhotoIterator photoIt2(photoIt);

    ++photoIt2;

    //photoIt should point to first row, photoIt2 to second one
    ASSERT_EQ("1", photoIt->getPath());
    ASSERT_EQ("2", photoIt2->getPath());
}
*/
