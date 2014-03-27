
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
        MOCK_METHOD1(getField, QVariant(Database::IQuery::Fields));
        MOCK_CONST_METHOD0(valid, bool());
        MOCK_METHOD0(clone, std::shared_ptr<IQuery>());
    };
}


TEST(PhotoIteratorShould, BeInvalidWhenConstructedWithNoArguments)
{
    Database::PhotoIterator photoIt;

    ASSERT_EQ(false, static_cast<bool>(photoIt));
    ASSERT_EQ(true, ! photoIt);
}


TEST(PhotoIteratorShould, beAbleToMoveToNextPhotoBasingOnPhotoId)
{
    auto query = std::make_shared<MockQuery>();

    using ::testing::Return;
    using ::testing::AtLeast;

    //initial condition in ++operator
    EXPECT_CALL(*query, valid()).Times(AtLeast(1)).WillOnce(Return(true));

    //first in constructor, when twice in ++operator until (from first row to second, and from second to third)
    EXPECT_CALL(*query, gotoNext()).Times(3).WillRepeatedly(Return(true));

    //called three times on each of rows
    EXPECT_CALL(*query, getField(Database::IQuery::Fields::Id))
        .Times(3)
        .WillOnce(Return(QVariant("1")))
        .WillOnce(Return(QVariant("1")))
        .WillOnce(Return(QVariant("2")));

    Database::PhotoIterator photoIt(query);

    ++photoIt;
}


TEST(PhotoIteratorShould, moveToFirstRowOfDataWhenConstructedWithQuery)
{
    auto query = std::make_shared<MockQuery>();

    using ::testing::Return;
    EXPECT_CALL(*query, gotoNext()).Times(1).WillOnce(Return(true));

    Database::PhotoIterator photoIt(query);
}


TEST(PhotoIteratorShould, BeCopyable)
{
    auto query = std::make_shared<MockQuery>();

    using ::testing::Return;
    using ::testing::AtLeast;


    EXPECT_CALL(*query, valid()).Times(AtLeast(1)).WillOnce(Return(true));

    //first in constructor, when once in ++operator
    EXPECT_CALL(*query, gotoNext()).Times(2).WillRepeatedly(Return(true));

    //called two times on each of rows
    EXPECT_CALL(*query, getField(Database::IQuery::Fields::Id))
        .Times(2)
        .WillOnce(Return(QVariant("1")))
        .WillOnce(Return(QVariant("2")));

    Database::PhotoIterator photoIt(query);
    Database::PhotoIterator photoIt2(photoIt);

    ++photoIt2;

    //photoIt should point to first row, photoIt2 to second one
    ///ASSERT_EQ("1", photoIt->
}
