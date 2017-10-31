
#include "qmodelindex_comparator.hpp"

#include <gmock/gmock.h>

#include "unit_tests_utils/mock_qabstractitemmodel.hpp"


TEST(QModelIndexComparatorTest, childrenOfSameParent)
{
    MockQAbstractItemModel model;
    QModelIndexComparator comparator;

    const QModelIndex c1 = model.constructIndex(1, 0);
    const QModelIndex c2 = model.constructIndex(11, 0);

    EXPECT_TRUE(comparator(c1, c2));    // c1 is less than c2
    EXPECT_FALSE(comparator(c2, c1));   // c2 is not less than c1
}
