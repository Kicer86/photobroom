
#include <gmock/gmock.h>

#include "containers_utils.hpp"


TEST(compareTest, removalDetection)
{
    const std::map<int, char> l =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'}
    };

    const std::map<int, char> r =
    {
        {1, 'a'},
    //  {2, 'b'},
        {3, 'c'}
    };

    std::vector<std::pair<int, char>> removed, added;
    std::vector<std::tuple<int, char, char>> changed;

    compare(l, r,
            std::back_inserter(removed),
            std::back_inserter(changed),
            std::back_inserter(added));

    EXPECT_TRUE(added.empty());
    EXPECT_TRUE(changed.empty());

    ASSERT_EQ(removed.size(), 1);
    EXPECT_EQ(removed.front().first, 2);
    EXPECT_EQ(removed.front().second, 'b');
}


TEST(compareTest, additionDetection)
{
    const std::map<int, char> l =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'}
    };

    const std::map<int, char> r =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'},
        {4, 'd'}
    };

    std::vector<std::pair<int, char>> removed, added;
    std::vector<std::tuple<int, char, char>> changed;

    compare(l, r,
            std::back_inserter(removed),
            std::back_inserter(changed),
            std::back_inserter(added));

    EXPECT_TRUE(removed.empty());
    EXPECT_TRUE(changed.empty());

    ASSERT_EQ(added.size(), 1);
    EXPECT_EQ(added.front().first, 4);
    EXPECT_EQ(added.front().second, 'd');
}


TEST(compareTest, changeDetection)
{
    const std::map<int, char> l =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'}
    };

    const std::map<int, char> r =
    {
        {1, 'a'},
        {2, 'q'},
        {3, 'c'}
    };

    std::vector<std::pair<int, char>> removed, added;
    std::vector<std::tuple<int, char, char>> changed;

    compare(l, r,
            std::back_inserter(removed),
            std::back_inserter(changed),
            std::back_inserter(added));

    EXPECT_TRUE(removed.empty());
    EXPECT_TRUE(added.empty());

    ASSERT_EQ(changed.size(), 1);
    EXPECT_EQ(std::get<0>(changed.front()), 2);
    EXPECT_EQ(std::get<1>(changed.front()), 'b');
    EXPECT_EQ(std::get<2>(changed.front()), 'q');
}

