
#include <gmock/gmock.h>

#include "unit_tests_utils/mock_qabstractitemmodel.hpp"
#include "qmodelindex_selector.hpp"

using testing::_;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;


struct QModelIndexSelectorTest: testing::Test
{
    QModelIndexSelectorTest()
    {
        p1 =  model.addItem(1, 0, top);
        p2 =  model.addItem(2, 0, top);

        c10 = model.addItem(0, 0, p1);
        c11 = model.addItem(1, 0, p1);
        c12 = model.addItem(2, 0, p1);

        c20 = model.addItem(0, 0, p2);
        c21 = model.addItem(1, 0, p2);
        c22 = model.addItem(2, 0, p2);
    }

    ~QModelIndexSelectorTest()
    {

    }

    NiceMock<StubQAbstractItemModel> model;

    QModelIndex top;
    QModelIndex p1;
    QModelIndex p2;

    QModelIndex c10;
    QModelIndex c11;
    QModelIndex c12;

    QModelIndex c20;
    QModelIndex c21;
    QModelIndex c22;
};


TEST_F(QModelIndexSelectorTest, childrenOfSameParent)
{
    const auto items = QModelIndexSelector::listAllBetween(c10, c12);

    EXPECT_EQ(items, std::vector<QModelIndex>({c10, c11, c12}));
}


TEST_F(QModelIndexSelectorTest, childrenOfDifferentParents)
{
    const auto items = QModelIndexSelector::listAllBetween(c11, c22);

    EXPECT_EQ(items, std::vector<QModelIndex>({c11, c12, p2, c20, c21, c22}));
}
