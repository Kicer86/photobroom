
#include <gmock/gmock.h>

#include "unit_tests_utils/mock_qabstractitemmodel.hpp"
#include "qmodelindex_selector.hpp"

using testing::_;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;


TEST(QModelIndexSelectorTest, childrenOfSameParent)
{
    NiceMock<MockQAbstractItemModel> model;

    const QModelIndex top;
    const QModelIndex c1 = model.constructIndex(1, 0);
    const QModelIndex c2 = model.constructIndex(3, 0);

    ON_CALL(model, parent(_))
        .WillByDefault(Return(top));

    ON_CALL(model, index(_, _, _))
        .WillByDefault(Invoke([&model](int r, int c, const QModelIndex &)
        {
            return model.constructIndex(r, c, nullptr);
        }));

    const auto items = QModelIndexSelector::listAllBetween(c1, c2);

    const std::size_t s = items.size();

    ASSERT_EQ(s, 3);
    for (std::size_t i = 0; i < s; i++)
    {
        EXPECT_EQ(items[i].row(), i + 1);
        EXPECT_EQ(items[i].column(), 0);
    }
}
