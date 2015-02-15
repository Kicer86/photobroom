
#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <core/tree.hpp>

template<typename T>
std::string dump(const tree<T>& tr)
{
    std::stringstream s;

    s << tr;

    return s.str();
}

TEST(treeTest, isConstructible)
{
    EXPECT_NO_THROW(
    {
        tree<int> tr;
    });
}



TEST(treeTest, isConstructedEmpty)
{
    tree<int> tr;

    EXPECT_EQ(true, tr.empty());
    EXPECT_EQ(true, tr.end() == tr.begin());
    EXPECT_EQ(0, tr.end() - tr.begin());
}


TEST(treeTest, acceptsRootLevelInserts)
{
    tree<int> tr;

    tr.insert(tr.begin(), 1);
    tr.insert(tr.begin(), 2);
    tr.insert(tr.begin(), 3);

    EXPECT_EQ(3, tr.end() - tr.begin());
    EXPECT_EQ("(3 2 1)", dump<int>(tr));

    tr.insert(tr.end(), 4);
    tr.insert(tr.end(), 5);
    tr.insert(tr.end(), 6);

    EXPECT_EQ(6, tr.end() - tr.begin());
    EXPECT_EQ("(3 2 1 4 5 6)", dump<int>(tr));
}


TEST(treeTest, acceptsRootLevelInsertsAtRandomLocations)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);               // (1)
    tr.insert(tr.end(), 3);               // (1 3)
    tr.insert(tr.begin() + 1, 2);         // (1 2 3)
    tr.insert(tr.begin() + 2, 8);         // (1 2 8 3)

    EXPECT_EQ(4, tr.end() - tr.begin());
    EXPECT_EQ("(1 2 8 3)", dump<int>(tr));
}


TEST(treeTest, acceptsChildLevelInserts)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);
    tr.insert(tr.end(), 2);
    tr.insert(tr.begin().children_begin(), 3);

    EXPECT_EQ(2, tr.end() - tr.begin());
    EXPECT_EQ("(1(3) 2)", dump<int>(tr));
}


TEST(treeTest, acceptsChildLevelInsertsAtRandomLocations)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    tr.insert(it.children_begin(), 4);              // (1 3(4))
    it = tr.insert(it.children_begin(), 2);         // (1 3(2 4))
    tr.insert(it.children_end(), 7);                // (1 3(2(7) 4))
    tr.insert(it.children_end(), 8);                // (1 3(2(7 8) 4))

    EXPECT_EQ(2, tr.end() - tr.begin());
    EXPECT_EQ("(1 3(2(7 8) 4))", dump<int>(tr));
}
