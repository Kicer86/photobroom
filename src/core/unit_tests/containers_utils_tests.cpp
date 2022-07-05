
#include <gmock/gmock.h>

#include "containers_utils.hpp"


using testing::ElementsAre;

TEST(compareTest, removalDetection)
{
    const std::map<int, char> l =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'},
        {4, 'd'},
        {5, 'e'}
    };

    const std::map<int, char> r =
    {
        {1, 'a'},
      //{2, 'b'},
        {3, 'c'},
      //{4, 'd'},
        {5, 'e'}
    };

    std::vector<std::pair<int, char>> removed, added;
    std::vector<std::tuple<int, char, char>> changed;

    compare(l, r,
            std::back_inserter(removed),
            std::back_inserter(changed),
            std::back_inserter(added));

    EXPECT_TRUE(added.empty());
    EXPECT_TRUE(changed.empty());

    ASSERT_EQ(removed.size(), 2);
    EXPECT_EQ(removed.front().first, 2);
    EXPECT_EQ(removed.front().second, 'b');
    EXPECT_EQ(removed.back().first, 4);
    EXPECT_EQ(removed.back().second, 'd');
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
        {0, '_'},
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

    ASSERT_EQ(added.size(), 2);
    EXPECT_EQ(added.front().first, 0);
    EXPECT_EQ(added.front().second, '_');
    EXPECT_EQ(added.back().first, 4);
    EXPECT_EQ(added.back().second, 'd');
}


TEST(compareTest, changeDetection)
{
    const std::map<int, char> l =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'},
        {4, 'd'},
    };

    const std::map<int, char> r =
    {
        {1, 'a'},
        {2, 'q'},
        {3, 'w'},
        {4, 'd'},
    };

    std::vector<std::pair<int, char>> removed, added;
    std::vector<std::tuple<int, char, char>> changed;

    compare(l, r,
            std::back_inserter(removed),
            std::back_inserter(changed),
            std::back_inserter(added));

    EXPECT_TRUE(removed.empty());
    EXPECT_TRUE(added.empty());

    ASSERT_EQ(changed.size(), 2);
    EXPECT_EQ(std::get<0>(changed.front()), 2);
    EXPECT_EQ(std::get<1>(changed.front()), 'b');
    EXPECT_EQ(std::get<2>(changed.front()), 'q');

    EXPECT_EQ(std::get<0>(changed.back()), 3);
    EXPECT_EQ(std::get<1>(changed.back()), 'c');
    EXPECT_EQ(std::get<2>(changed.back()), 'w');
}


TEST(compareTest, mixDetection)
{
    const std::map<int, char> l =
    {
        {1, 'a'},
        {2, 'b'},
        {3, 'c'},
        {4, 'd'},
        {5, 'e'},
        {6, 'f'},
    };

    const std::map<int, char> r =
    {
        {0, '_'},
        {1, 'a'},
    //  {2, 'b'},
        {3, 'q'},
    //  {4, 'd'},
        {5, 'e'},
        {6, 'w'},
        {7, '_'}
    };

    std::vector<std::pair<int, char>> removed, added;
    std::vector<std::tuple<int, char, char>> changed;

    compare(l, r,
            std::back_inserter(removed),
            std::back_inserter(changed),
            std::back_inserter(added));


    // removal
    ASSERT_EQ(removed.size(), 2);
    EXPECT_EQ(removed.front().first, 2);
    EXPECT_EQ(removed.front().second, 'b');
    EXPECT_EQ(removed.back().first, 4);
    EXPECT_EQ(removed.back().second, 'd');

    // addition
    ASSERT_EQ(added.size(), 2);
    EXPECT_EQ(added.front().first, 0);
    EXPECT_EQ(added.front().second, '_');
    EXPECT_EQ(added.back().first, 7);
    EXPECT_EQ(added.back().second, '_');

    // change
    ASSERT_EQ(changed.size(), 2);
    EXPECT_EQ(std::get<0>(changed.front()), 3);
    EXPECT_EQ(std::get<1>(changed.front()), 'c');
    EXPECT_EQ(std::get<2>(changed.front()), 'q');

    EXPECT_EQ(std::get<0>(changed.back()), 6);
    EXPECT_EQ(std::get<1>(changed.back()), 'f');
    EXPECT_EQ(std::get<2>(changed.back()), 'w');
}


TEST(VectorAdditionTest, firstShorter)
{
    const std::vector<int> l = {1, 2, 4, 8};
    const std::vector<int> r = {2, 4, 8, 16, 32};
    const std::vector<int> s = l + r;

    ASSERT_EQ(s.size(), 5);

    const std::vector<int> expected_sum{3, 6, 12, 24, 32};
    EXPECT_EQ(s, expected_sum);
}


TEST(VectorAdditionTest, secondShorter)
{
    const std::vector<int> l = {1, 2, 4, 8, 16, 32};
    const std::vector<int> r = {2, 4, 8, 16, 32};
    const std::vector<int> s = l + r;

    ASSERT_EQ(s.size(), 6);

    const std::vector<int> expected_sum{3, 6, 12, 24, 48, 32};
    EXPECT_EQ(s, expected_sum);
}


TEST(VectorAdditionAndAssignTest, firstShorter)
{
    std::vector<int> l = {1, 2, 4, 8};
    const std::vector<int> r = {2, 4, 8, 16, 32};

    l += r;

    ASSERT_EQ(l.size(), 5);

    const std::vector<int> expected_sum{3, 6, 12, 24, 32};
    EXPECT_EQ(l, expected_sum);
}


TEST(VectorAdditionAndAssignTest, secondShorter)
{
    std::vector<int> l = {1, 2, 4, 8, 16, 32};
    const std::vector<int> r = {2, 4, 8, 16, 32};

    l += r;

    ASSERT_EQ(l.size(), 6);

    const std::vector<int> expected_sum{3, 6, 12, 24, 48, 32};
    EXPECT_EQ(l, expected_sum);
}


TEST(VectorDivisionTest, byInt)
{
    const std::vector<int> l = {2, 4, 8, 16, 32};
    const std::vector r = l / 2;

    ASSERT_EQ(r.size(), 5);

    const std::vector<int> expected_sum{1, 2, 4, 8, 16};
    EXPECT_EQ(r, expected_sum);
}


TEST(VectorDivisionAndAssignTest, byInt)
{
    std::vector l = {2, 4, 8, 16, 32};
    l /= 2;

    ASSERT_EQ(l.size(), 5);

    const std::vector expected_sum = {1, 2, 4, 8, 16};
    EXPECT_EQ(l, expected_sum);
}


TEST(RemoveUniqueTest, mix)
{
    {
        std::vector<int> empty;
        empty.erase(remove_unique(empty.begin(), empty.end()), empty.end());
        EXPECT_TRUE(empty.empty());
    }

    {
        std::vector<int> singleUnique = {5};
        singleUnique.erase(remove_unique(singleUnique.begin(), singleUnique.end()), singleUnique.end());
        EXPECT_TRUE(singleUnique.empty());
    }

    {
        std::vector<int> doubleUnique = {5, 9};
        doubleUnique.erase(remove_unique(doubleUnique.begin(), doubleUnique.end()), doubleUnique.end());
        EXPECT_TRUE(doubleUnique.empty());
    }

    {
        std::vector<int> singleNonUnique = {5, 5};
        singleNonUnique.erase(remove_unique(singleNonUnique.begin(), singleNonUnique.end()), singleNonUnique.end());
        EXPECT_THAT(singleNonUnique, ElementsAre(5, 5));
    }

    {
        std::vector<int> nonUniqueSurrounded = {1, 5, 5, 9};
        nonUniqueSurrounded.erase(remove_unique(nonUniqueSurrounded.begin(), nonUniqueSurrounded.end()), nonUniqueSurrounded.end());
        EXPECT_THAT(nonUniqueSurrounded, ElementsAre(5, 5));
    }

    {
        std::vector<int> doubleNonUnique = {1, 1, 9, 9};
        doubleNonUnique.erase(remove_unique(doubleNonUnique.begin(), doubleNonUnique.end()), doubleNonUnique.end());
        EXPECT_THAT(doubleNonUnique, ElementsAre(1, 1, 9, 9));
    }

    {
        std::vector<int> nonUniqueSplit = {1, 1, 5, 9, 9};
        nonUniqueSplit.erase(remove_unique(nonUniqueSplit.begin(), nonUniqueSplit.end()), nonUniqueSplit.end());
        EXPECT_THAT(nonUniqueSplit, ElementsAre(1, 1, 9, 9));
    }
}
