
#include <gmock/gmock.h>

#include "ptr_iterator.hpp"


typedef std::vector<std::unique_ptr<int>> UPtrVector;

TEST(ptr_iteratorTest, constructible)
{
    UPtrVector vector;

    ptr_iterator<UPtrVector> iterator;
}


TEST(ptr_iteratorTest, assignment)
{
    UPtrVector vector;

    ptr_iterator<UPtrVector> c_iterator(vector.cbegin());
    ptr_iterator<UPtrVector> iterator(vector.begin());

    ptr_iterator<UPtrVector> c_iterator2 = vector.cbegin();
}


TEST(ptr_iteratorTest, dereference)
{
    UPtrVector vector;
    vector.emplace_back(new int(123));

    ptr_iterator<UPtrVector> iterator(vector.cbegin());

    const int* v = *iterator;

    EXPECT_EQ(*v, 123);
}


TEST(ptr_iteratorTest, preincrementation)
{
    UPtrVector vector;
    vector.emplace_back(new int(123));
    vector.emplace_back(new int(234));
    vector.emplace_back(new int(345));

    ptr_iterator<UPtrVector> iterator(vector.cbegin());

    const int* v1 = *iterator;
    ++iterator;
    const int* v2 = *iterator;
    ++iterator;
    const int* v3 = *iterator;

    EXPECT_EQ(*v1, 123);
    EXPECT_EQ(*v2, 234);
    EXPECT_EQ(*v3, 345);
}


TEST(ptr_iteratorTest, equal_operator)
{
    UPtrVector vector;
    vector.emplace_back(new int(123));
    vector.emplace_back(new int(234));
    vector.emplace_back(new int(345));

    ptr_iterator<UPtrVector> iterator(vector.cbegin());
    ptr_iterator<UPtrVector> iterator2(vector.cbegin());

    EXPECT_EQ(iterator, iterator2);
    EXPECT_EQ(iterator, vector.cbegin());

    ++iterator;
    ++iterator2;

    ++iterator;
    ++iterator2;

    EXPECT_EQ(iterator, iterator2);

    ++iterator;
    ++iterator2;

    EXPECT_EQ(iterator, iterator2);
    EXPECT_EQ(iterator, vector.cend());
}
