
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

    signed char a = std::numeric_limits< char >::max();
    a++;

    int i = INT_MIN;
    int j = -i;

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

    tr.insert(tr.end(), 1);           // (1)
    tr.insert(tr.end(), 3);           // (1 3)
    tr.insert(tree<int>::level_iterator(tr.begin()) + 1, 2);     // (1 2 3)
    tr.insert(tree<int>::level_iterator(tr.begin()) + 2, 8);     // (1 2 8 3)

    EXPECT_EQ(4, tr.end() - tr.begin());
    EXPECT_EQ("(1 2 8 3)", dump<int>(tr));
}


TEST(treeTest, acceptsChildLevelInserts)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);
    tr.insert(tr.end(), 2);
    tr.insert( tree<int>::level_iterator(tr.begin()).begin(), 3);

    EXPECT_EQ(3, tr.end() - tr.begin());
    EXPECT_EQ("(1(3) 2)", dump<int>(tr));
}


TEST(treeTest, acceptsChildLevelInsertsAtRandomLocations)
{
    tree<int> tr;

    typedef tree<int>::level_iterator flat;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    tr.insert( flat(it).begin(), 4);                // (1 3(4))
    it = tr.insert( flat(it).begin(), 2);           // (1 3(2 4))
    tr.insert( flat(it).end(), 7);                  // (1 3(2(7) 4))
    tr.insert( flat(it).end(), 8);                  // (1 3(2(7 8) 4))

    EXPECT_EQ(6, tr.end() - tr.begin());
    EXPECT_EQ("(1 3(2(7 8) 4))", dump<int>(tr));
}


TEST(treeTest, iterationsOverConstTree)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);
    tr.insert(tr.end(), 2);
    tr.insert(tr.end(), 3);
    tr.insert(tr.end(), 4);

    const tree<int>& c_tr = tr;

    int s = 0;
    for(auto it = c_tr.cbegin(); it != c_tr.cend(); ++it)
        s += *it;

    EXPECT_EQ(10, s);
}


TEST(treeTest, randomIteratorsGoThrouHierarchy)
{
    tree<int> tr;
    typedef tree<int>::level_iterator flat;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    tr.insert( flat(it).begin(), 4);                // (1 3(4))
    it = tr.insert( flat(it).begin(), 2);           // (1 3(2 4))
    tr.insert( flat(it).end(), 7);                  // (1 3(2(7) 4))
    tr.insert( flat(it).end(), 8);                  // (1 3(2(7 8) 4))

    std::vector<int> values;
    for(int v: tr)
        values.push_back(v);

    EXPECT_EQ( std::vector<int>({1, 3, 2, 7, 8, 4}), values);
}


TEST(treeTest, forwardTraversingThrouTree)
{
    tree<int> tr;
    typedef tree<int>::level_iterator flat;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    it = tr.insert(flat(it).begin(), 4);            // (1 3(4))
    tr.insert(flat(it).end(), 9);                   // (1 3(4 (9)))
    tr.insert(flat(it).end(), 10);                  // (1 3(4 (9 10)))

    it = tr.insert(flat(it).begin(), 2);            // (1 3(4(2 9 10)))
    tr.insert(flat(it).end(), 7);                   // (1 3(4(2(7) 9 10)))
    tr.insert(flat(it).end(), 8);                   // (1 3(4(2(7 8) 9 10)))

    EXPECT_EQ(8, tr.end() - tr.begin());
    EXPECT_EQ("(1 3(4(2(7 8) 9 10)))", dump<int>(tr));

    std::vector<int> v;
    for(auto n: tr)
        v.push_back(n);

    EXPECT_EQ(std::vector<int>({1, 3, 4, 2, 7, 8, 9, 10}), v);
}


TEST(treeTest, forwardTraversingThrouTree2)
{
    tree<int> tr;
    typedef tree<int>::level_iterator flat;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    auto it2 = tr.insert(flat(it).begin(), 4);      // (1 3(4))
    tr.insert(flat(it2).end(), 9);                  // (1 3(4 (9)))
    tr.insert(flat(it2).end(), 10);                 // (1 3(4 (9 10)))

    it = tr.insert(flat(it).begin(), 2);            // (1 3(2 4(9 10)))
    tr.insert(flat(it).end(), 7);                   // (1 3(2(7) 4(9 10)))
    tr.insert(flat(it).end(), 8);                   // (1 3(2(7 8) 4(9 10)))

    EXPECT_EQ(8, tr.end() - tr.begin());
    EXPECT_EQ("(1 3(2(7 8) 4(9 10)))", dump<int>(tr));

    std::vector<int> v;
    for(auto n: tr)
        v.push_back(n);

    EXPECT_EQ(std::vector<int>({1, 3, 2, 7, 8, 4, 9, 10}), v);
}


TEST(treeTest, ConstIteratorAreConst)
{
    typedef tree<int>::const_iterator       const_flat;
    typedef tree<int>::const_level_iterator const_level;

    EXPECT_EQ(true, std::is_const<const_flat::ValueType>::value);
    EXPECT_EQ(true, std::is_const<const_level::ValueType>::value);
}

/*
TEST(treeTest, reverseTraversingThrouTree)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    it = tr.insert(it->begin(), 4);                 // (1 3(4))
    tr.insert(it->end(), 9);                        // (1 3(4 (9)))
    tr.insert(it->end(), 10);                       // (1 3(4 (9 10)))

    it = tr.insert(it->begin(), 2);                 // (1 3(4(2 9 10)))
    tr.insert(it->end(), 7);                        // (1 3(4(2(7) 9 10)))
    tr.insert(it->end(), 8);                        // (1 3(4(2(7 8) 9 10)))

    EXPECT_EQ(8, tr.end() - tr.begin());
    EXPECT_EQ("(1 3(4(2(7 8) 9 10)))", dump<int>(tr));

    std::vector<int> v;
    for(auto it = tr.end(); : tr)
        v.push_back(*n);

    EXPECT_EQ(std::vector<int>({1, 3, 4, 2, 7, 8, 9, 10}), v);
}


TEST(treeTest, reverseTraversingThrouTree2)
{
    tree<int> tr;

    tr.insert(tr.end(), 1);                         // (1)
    auto it = tr.insert(tr.end(), 3);               // (1 3)

    auto it2 = tr.insert(it->begin(), 4);           // (1 3(4))
    tr.insert(it2->end(), 9);                       // (1 3(4 (9)))
    tr.insert(it2->end(), 10);                      // (1 3(4 (9 10)))

    it = tr.insert(it->begin(), 2);                 // (1 3(2 4(9 10)))
    tr.insert(it->end(), 7);                        // (1 3(2(7) 4(9 10)))
    tr.insert(it->end(), 8);                        // (1 3(2(7 8) 4(9 10)))

    EXPECT_EQ(8, tr.end() - tr.begin());
    EXPECT_EQ("(1 3(2(7 8) 4(9 10)))", dump<int>(tr));

    std::vector<int> v;
    for(auto n: tr)
        v.push_back(*n);

    EXPECT_EQ(std::vector<int>({1, 3, 2, 7, 8, 4, 9, 10}), v);
}
*/
