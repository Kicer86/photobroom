
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <core/tree.hpp>


TEST(treeTest, isConstructible)
{
    EXPECT_NO_THROW(
    {
        tree tr;
    });
}



TEST(treeTest, isConstructedEmpty)
{
    tree tr;

    EXPECT_EQ(nullptr, tr.first());
    EXPECT_EQ(nullptr, tr.last());
}


TEST(treeTest, allowsToSetFirstItem)
{
    tree tr;

    tree::item* i = new tree::item;
    tr.insert_front(i);

    EXPECT_EQ(i, tr.first());
    EXPECT_EQ(i, tr.last());

    tree::item* i2 = new tree::item;
    tr.insert_front(i2);

    EXPECT_EQ(i2, tr.first());
    EXPECT_EQ(i, tr.last());

    delete i;
    delete i2;
}


TEST(treeTest, allowsToSetLastItem)
{
    tree tr;

    tree::item* i = new tree::item;
    tr.insert_back(i);

    EXPECT_EQ(i, tr.first());
    EXPECT_EQ(i, tr.last());

    tree::item* i2 = new tree::item;
    tr.insert_back(i2);

    EXPECT_EQ(i, tr.first());
    EXPECT_EQ(i2, tr.last());

    delete i;
    delete i2;
}


TEST(treeTest, removesDeletedItem)
{
    tree tr;

    tree::item* i = new tree::item;
    tr.insert_front(i);

    delete i;

    EXPECT_EQ(nullptr, tr.first());
    EXPECT_EQ(nullptr, tr.last());
}


TEST(treeTest, dealsRightWithInsertionsOnRootLevel)
{
    tree tr;

    tree::item* i1 = new tree::item;
    tree::item* i2 = new tree::item;
    tree::item* i3 = new tree::item;
    tree::item* i4 = new tree::item;
    tree::item* i5 = new tree::item;
    tree::item* i6 = new tree::item;

    tr.insert_front(i3);
    tr.insert_back(i5);
    i5->insert_before(i4);
    tr.insert_front(i2);
    i2->insert_before(i1);
    i5->insert_after(i6);

    EXPECT_EQ(nullptr, i1->previous());
    EXPECT_EQ(i2, i1->next());

    EXPECT_EQ(i1, i2->previous());
    EXPECT_EQ(i3, i2->next());

    EXPECT_EQ(i2, i3->previous());
    EXPECT_EQ(i4, i3->next());

    EXPECT_EQ(i3, i4->previous());
    EXPECT_EQ(i5, i4->next());

    EXPECT_EQ(i4, i5->previous());
    EXPECT_EQ(i6, i5->next());

    EXPECT_EQ(i5, i6->previous());
    EXPECT_EQ(nullptr, i6->next());
}
