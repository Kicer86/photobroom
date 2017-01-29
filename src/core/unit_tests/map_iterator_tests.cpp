
#include <gmock/gmock.h>

#include "map_iterator.hpp"


typedef std::map<int, char> IntCharMap;

TEST(map_iteratorTest, constructible)
{
    IntCharMap map;

    key_map_iterator<IntCharMap> iterator;
}


TEST(map_iteratorTest, assignment)
{
    IntCharMap map;

    key_map_iterator<IntCharMap> c_iterator(map.cbegin());
    key_map_iterator<IntCharMap> iterator(map.begin());

    key_map_iterator<IntCharMap> c_iterator2 = map.cbegin();
}


TEST(map_iteratorTest, dereference)
{
    IntCharMap map = { {123, 'c'} };

    key_map_iterator<IntCharMap> iterator(map.cbegin());

    const int v = *iterator;

    EXPECT_EQ(v, 123);
}


TEST(map_iteratorTest, preincrementation)
{
    IntCharMap map = { {123, 'c'}, {234, 'd'}, {345, 'e'} };

    key_map_iterator<IntCharMap> iterator(map.cbegin());

    const int v1 = *iterator;
    ++iterator;
    const int v2 = *iterator;
    ++iterator;
    const int v3 = *iterator;

    EXPECT_EQ(v1, 123);
    EXPECT_EQ(v2, 234);
    EXPECT_EQ(v3, 345);
}


TEST(map_iteratorTest, equal_operator)
{
    IntCharMap map = { {123, 'c'}, {234, 'd'}, {345, 'e'} };

    key_map_iterator<IntCharMap> iterator(map.cbegin());
    key_map_iterator<IntCharMap> iterator2(map.cbegin());

    EXPECT_EQ(iterator, iterator2);
    EXPECT_EQ(iterator, map.cbegin());

    ++iterator;
    ++iterator2;

    ++iterator;
    ++iterator2;

    EXPECT_EQ(iterator, iterator2);

    ++iterator;
    ++iterator2;

    EXPECT_EQ(iterator, iterator2);
    EXPECT_EQ(iterator, map.cend());
}
