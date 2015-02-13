
#include <core/tree.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


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
    EXPECT_EQ("(3 2 1)", tr.dump());

    tr.insert(tr.end(), 4);
    tr.insert(tr.end(), 5);
    tr.insert(tr.end(), 6);

    EXPECT_EQ(6, tr.end() - tr.begin());
    EXPECT_EQ("(3 2 1 4 5 6)", tr.dump());
}
