
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
        p3 =  model.addItem(3, 0, top);

        c10 = model.addItem(0, 0, p1);
        c11 = model.addItem(1, 0, p1);
        c12 = model.addItem(2, 0, p1);

        c20 = model.addItem(0, 0, p2);
        c21 = model.addItem(1, 0, p2);
        c22 = model.addItem(2, 0, p2);

        c30 = model.addItem(0, 0, p3);
        c31 = model.addItem(1, 0, p3);
        c32 = model.addItem(2, 0, p3);

        c300 = model.addItem(0, 0, c30);
        c301 = model.addItem(1, 0, c30);
        c302 = model.addItem(2, 0, c30);

        c310 = model.addItem(0, 0, c31);
        c311 = model.addItem(1, 0, c31);
        c312 = model.addItem(2, 0, c31);

        c320 = model.addItem(0, 0, c32);
        c321 = model.addItem(1, 0, c32);
        c322 = model.addItem(2, 0, c32);
    }

    ~QModelIndexSelectorTest()
    {

    }

    NiceMock<StubQAbstractItemModel> model;

    QModelIndex top;
    QModelIndex p1;
    QModelIndex p2;
    QModelIndex p3;

    QModelIndex c10;
    QModelIndex c11;
    QModelIndex c12;

    QModelIndex c20;
    QModelIndex c21;
    QModelIndex c22;

    QModelIndex c30;
    QModelIndex c31;
    QModelIndex c32;

    QModelIndex c300;
    QModelIndex c301;
    QModelIndex c302;

    QModelIndex c310;
    QModelIndex c311;
    QModelIndex c312;

    QModelIndex c320;
    QModelIndex c321;
    QModelIndex c322;
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


TEST_F(QModelIndexSelectorTest, childrenWithChildren)
{
    const auto items = QModelIndexSelector::listAllBetween(c31, c32);

    EXPECT_EQ(items, std::vector<QModelIndex>({c31, c310, c311, c312, c32}));
}
