
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include <configuration/constants.hpp>

#include "model_view/view_helpers/positions_calculator.hpp"
#include "model_view/view_helpers/data.hpp"

#include "mock_configuration.hpp"
#include "mock_qabstractitemmodel.hpp"

TEST(PositionsCalculatorShould, BeConstructable)
{
    using ::testing::_;
    using ::testing::Return;

    EXPECT_NO_THROW({
        MockConfiguration config;
        MockQAbstractItemModel model;

        EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));

        Data data;
        data.m_configuration = &config;

        PositionsCalculator calculator(&model, &data, 100);
    });
}



TEST(PositionsCalculatorShould, KeepTopItemSizeEmptyWhenModelIsEmpty)
{
    using ::testing::_;
    using ::testing::Return;

    QModelIndex top;

    MockConfiguration config;
    MockQAbstractItemModel model;

    EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));
    EXPECT_CALL(model, rowCount(top)).Times(1).WillOnce(Return(0));

    Data data;
    data.m_configuration = &config;

    PositionsCalculator calculator(&model, &data, 100);
    calculator.updateItems();

    const ModelIndexInfo info = data.get(top);

    EXPECT_EQ(info.getRect(), QRect());
    EXPECT_EQ(info.getOverallRect(), QRect());
}


TEST(PositionsCalculatorShould, SetTopItemsSizeToEmptyEvenIfThereIsAChild)
{
    using ::testing::_;
    using ::testing::Return;
    using ::testing::Invoke;

    QModelIndex top;

    struct Item
    {

    };

    // 1 main node + 2 children
    Item top_child1;                     //0, 0
    Item top_child1_child1;              //    0, 0
    Item top_child1_child2;              //    1, 0

    MockConfiguration config;
    MockQAbstractItemModel model;

    QModelIndex top_child1_idx = model.createIndex(0, 0, &top_child1);
    QModelIndex top_child1_child1_idx = model.createIndex(0, 0, &top_child1_child1);
    QModelIndex top_child1_child2_idx = model.createIndex(1, 0, &top_child1_child2);

    auto createIndex = [&](int r, int c, const QModelIndex& p)
    {
        QModelIndex result;
        EXPECT_EQ(true, p == top || p == top_child1_idx);

        if (p == top)
        {
            EXPECT_EQ(r, 0);
            EXPECT_EQ(c, 0);
            result = top_child1_idx;
        }
        else if (p == top_child1_idx)
        {
            EXPECT_EQ(c, 0);
            EXPECT_EQ(true, r == 0 | r == 1);

            if (r == 0)
                result = top_child1_child1_idx;
            else if (r == 1)
                result = top_child1_child2_idx;
        }

        return result;
    };

    auto parent = [&](const QModelIndex& idx)
    {
        QModelIndex result;

        if (idx == top_child1_idx)
            result = top;

        return result;
    };

    EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));

    EXPECT_CALL(model, rowCount(top)).WillRepeatedly(Return(1));
    EXPECT_CALL(model, rowCount(top_child1_idx)).WillRepeatedly(Return(2));
    EXPECT_CALL(model, rowCount(top_child1_child1_idx)).WillRepeatedly(Return(0));
    EXPECT_CALL(model, rowCount(top_child1_child2_idx)).WillRepeatedly(Return(0));

    EXPECT_CALL(model, columnCount(_)).WillRepeatedly(Return(1));              // one column
    EXPECT_CALL(model, index(0, 0, top)).WillRepeatedly(Invoke(createIndex));
    EXPECT_CALL(model, parent(_)).WillRepeatedly(Invoke(parent));

    QModelIndex child = model.index(0, 0, top);

    Data data;
    data.m_configuration = &config;

    PositionsCalculator calculator(&model, &data, 100);
    calculator.updateItems();

    {
        const ModelIndexInfo info = data.get(top);

        EXPECT_EQ(info.getRect(), QRect());                        //invisible
        EXPECT_EQ(info.getOverallRect(), QRect(0, 0, 100, 40));    //but has overall size of all items
    }

    {
        const ModelIndexInfo info = data.get(top_child1_idx);

        EXPECT_EQ(info.getRect(), QRect(0, 0, 100, 40));           // its position
        EXPECT_EQ(info.getOverallRect(), QRect(0, 0, 100, 40));    // no children expanded - overall == size
    }
}
