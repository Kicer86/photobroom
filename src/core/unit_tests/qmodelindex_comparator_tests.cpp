
#include <gmock/gmock.h>

#include "unit_tests_utils/mock_qabstractitemmodel.hpp"
#include "qmodelindex_comparator.hpp"

using testing::_;
using testing::NiceMock;
using testing::Return;

TEST(QModelIndexComparatorTest, childrenOfSameParent)
{
    NiceMock<MockQAbstractItemModel> model;
    QModelIndexComparator comparator;

    const QModelIndex topParent;     // parent for c1 and c2
    const QModelIndex c1 = model.constructIndex(1, 0);
    const QModelIndex c2 = model.constructIndex(11, 0);

    ON_CALL(model, parent(_))
        .WillByDefault(Return(topParent));

    EXPECT_TRUE(comparator(c1, c2));    // c1 is less than c2
    EXPECT_FALSE(comparator(c2, c1));   // c2 is not less than c1
}


TEST(QModelIndexComparatorTest, childrenOfDifferentParents)
{
    MockQAbstractItemModel model;
    QModelIndexComparator comparator;

    const QModelIndex top;

    const QModelIndex p1 = model.constructIndex(5, 0);
    const QModelIndex p2 = model.constructIndex(7, 0);

    const QModelIndex c1 = model.constructIndex(1, 0);
    const QModelIndex c2 = model.constructIndex(11, 0);

    ON_CALL(model, parent(_))
        .WillByDefault(Return(top));

    ON_CALL(model, parent(c1))
        .WillByDefault(Return(p2));

    ON_CALL(model, parent(c2))
        .WillByDefault(Return(p1));

    // c1 is children of p2
    // c2 is children of p1
    // p1 is first, so its child should win

    EXPECT_TRUE(comparator(c2, c1));    // c2 is less than c1
    EXPECT_FALSE(comparator(c1, c2));   // c1 is not less than c2
}
