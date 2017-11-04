
#include <gmock/gmock.h>

#include "unit_tests_utils/mock_qabstractitemmodel.hpp"
#include "qmodelindex_selector.hpp"

using testing::_;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;


TEST(QModelIndexSelectorTest, childrenOfSameParent)
{
    NiceMock<StubQAbstractItemModel> model;

    const QModelIndex top;
    const QModelIndex c1 = model.addItem(1, 0, top);
                           model.addItem(2, 0, top);
    const QModelIndex c2 = model.addItem(3, 0, top);

    const auto items = QModelIndexSelector::listAllBetween(c1, c2);
    const std::size_t s = items.size();

    ASSERT_EQ(s, 3);
    for (std::size_t i = 0; i < s; i++)
    {
        EXPECT_EQ(items[i].row(), i + 1);
        EXPECT_EQ(items[i].column(), 0);
    }
}



TEST(QModelIndexSelectorTest, childrenOfDifferentParents)
{
    NiceMock<StubQAbstractItemModel> model;

    const QModelIndex top;
    const QModelIndex p1 =  model.addItem(1, 0, top);
    const QModelIndex p2 =  model.addItem(2, 0, top);

                            model.addItem(0, 0, p1);
    const QModelIndex c11 = model.addItem(1, 0, p1);
    const QModelIndex c12 = model.addItem(2, 0, p1);

    const QModelIndex c20 = model.addItem(0, 0, p2);
    const QModelIndex c21 = model.addItem(1, 0, p2);
    const QModelIndex c22 = model.addItem(2, 0, p2);

    const auto items = QModelIndexSelector::listAllBetween(c11, c22);

    EXPECT_EQ(items, std::vector<QModelIndex>({c11, c12, p2, c20, c21, c22}));
}
