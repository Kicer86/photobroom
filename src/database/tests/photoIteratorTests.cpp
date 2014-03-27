
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
        MOCK_METHOD1(getField, QVariant(const QString &));
        MOCK_CONST_METHOD0(valid, bool());
        MOCK_METHOD0(clone, std::shared_ptr<IQuery>());
    };
}


TEST(PhotoIteratorShould, BeInvalidWhenConstructedWithNoArguments)
{
    Database::PhotoIterator photoIt;

    ASSERT_EQ(true, static_cast<bool>(photoIt));
}
