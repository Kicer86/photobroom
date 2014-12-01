
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include <configuration/constants.hpp>

#include <Qt5/model_view/view_helpers/positions_calculator.hpp>
#include <Qt5/model_view/view_helpers/data.hpp>

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

    struct Item
    {

    };

    MockConfiguration config;
    MockQAbstractItemModel model;

    // top + 1 main node + 2 children
    Item top_child1;                     //0, 0
    Item top_child1_child1;              //    0, 0
    Item top_child1_child2;              //    1, 0

    QModelIndex top_idx;
    QModelIndex top_child1_idx = model.createIndex(0, 0, &top_child1);
    QModelIndex top_child1_child1_idx = model.createIndex(0, 0, &top_child1_child1);
    QModelIndex top_child1_child2_idx = model.createIndex(1, 0, &top_child1_child2);

    auto createIndex = [&](int r, int c, const QModelIndex& p)
    {
        QModelIndex result;
        EXPECT_EQ(true, p == top_idx || p == top_child1_idx);

        if (p == top_idx)
        {
            EXPECT_EQ(r, 0);
            EXPECT_EQ(c, 0);
            result = top_child1_idx;
        }
        else if (p == top_child1_idx)
        {
            EXPECT_EQ(c, 0);
            EXPECT_EQ(true, r == 0 || r == 1);

            if (r == 0)
                result = top_child1_child1_idx;
            else if (r == 1)
                result = top_child1_child2_idx;
        }

        return result;
    };

    auto parent = [&](const QModelIndex& idx)
    {
        EXPECT_EQ(idx, top_child1_idx);

        QModelIndex result;

        if (idx == top_child1_idx)
            result = top_idx;

        return result;
    };

    EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));

    EXPECT_CALL(model, rowCount(top_idx)).WillRepeatedly(Return(1));
    EXPECT_CALL(model, rowCount(top_child1_idx)).WillRepeatedly(Return(2));
    EXPECT_CALL(model, rowCount(top_child1_child1_idx)).WillRepeatedly(Return(0));
    EXPECT_CALL(model, rowCount(top_child1_child2_idx)).WillRepeatedly(Return(0));

    EXPECT_CALL(model, columnCount(_)).WillRepeatedly(Return(1));              // one column
    EXPECT_CALL(model, index(0, 0, top_idx)).WillRepeatedly(Invoke(createIndex));
    EXPECT_CALL(model, parent(_)).WillRepeatedly(Invoke(parent));

    Data data;
    data.m_configuration = &config;

    PositionsCalculator calculator(&model, &data, 100);
    calculator.updateItems();

    {
        const ModelIndexInfo info = data.get(top_idx);

        EXPECT_EQ(info.getRect(), QRect());                        //invisible
        EXPECT_EQ(info.getOverallRect(), QRect(0, 0, 100, 40));    //but has overall size of all items
    }

    {
        const ModelIndexInfo info = data.get(top_child1_idx);

        EXPECT_EQ(info.getRect(), QRect(0, 0, 100, 40));           // its position
        EXPECT_EQ(info.getOverallRect(), QRect(0, 0, 100, 40));    // no children expanded - overall == size
    }
}




TEST(PositionsCalculatorShould, SetMainNodesSizeToCoverItsChild)
{
    using ::testing::_;
    using ::testing::Return;
    using ::testing::Invoke;

    struct Item
    {

    };

    MockConfiguration config;
    MockQAbstractItemModel model;

    // 1 main node + 2 children
    Item top_child1;                     //0, 0
    Item top_child1_child1;              //    0, 0
    Item top_child1_child2;              //    1, 0

    QModelIndex top_idx;
    QModelIndex top_child1_idx = model.createIndex(0, 0, &top_child1);
    QModelIndex top_child1_child1_idx = model.createIndex(0, 0, &top_child1_child1);
    QModelIndex top_child1_child2_idx = model.createIndex(1, 0, &top_child1_child2);

    auto createIndex = [&](int r, int c, const QModelIndex& p)
    {
        QModelIndex result;
        EXPECT_EQ(true, p == top_idx || p == top_child1_idx);

        if (p == top_idx)
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
        EXPECT_EQ(true, idx == top_child1_idx || idx == top_child1_child1_idx || idx == top_child1_child2_idx);

        QModelIndex result;

        if (idx == top_child1_idx)
            result = top_idx;
        else if (idx == top_child1_child1_idx || idx == top_child1_child2_idx)
            result = top_child1_idx;

        return result;
    };

    auto data = [&](const QModelIndex& idx, int d)
    {
        QVariant result;
        EXPECT_EQ(true, idx == top_child1_child1_idx || idx == top_child1_child2_idx);

        if (d == Qt::DecorationRole)
            result = QPixmap(100, 50);
        else if (d == Qt::DisplayRole)
            result = "Empty";

        return result;
    };

    EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));

    EXPECT_CALL(model, rowCount(top_idx)).WillRepeatedly(Return(1));
    EXPECT_CALL(model, rowCount(top_child1_idx)).WillRepeatedly(Return(2));
    EXPECT_CALL(model, rowCount(top_child1_child1_idx)).WillRepeatedly(Return(0));
    EXPECT_CALL(model, rowCount(top_child1_child2_idx)).WillRepeatedly(Return(0));

    EXPECT_CALL(model, columnCount(_)).WillRepeatedly(Return(1));              // one column
    EXPECT_CALL(model, index(_, _, _)).WillRepeatedly(Invoke(createIndex));
    EXPECT_CALL(model, parent(_)).WillRepeatedly(Invoke(parent));
    EXPECT_CALL(model, data(_, _)).WillRepeatedly(Invoke(data));

    Data view_data;
    view_data.m_configuration = &config;

    //expand main node to show children
    ModelIndexInfo info = view_data.get(top_child1_idx);
    info.expanded = true;
    view_data.update(info);

    PositionsCalculator calculator(&model, &view_data, 100);
    calculator.updateItems();

    {
        const ModelIndexInfo info = view_data.get(top_idx);

        EXPECT_EQ(info.getRect(), QRect());                        //invisible
        EXPECT_EQ(info.getOverallRect(), QRect(0, 0, 100, 40));    //but has overall size of all items
    }

    {
        const ModelIndexInfo info = view_data.get(top_child1_idx);

        EXPECT_EQ(info.getRect(), QRect(0, 0, 100, 40));           // its position
        EXPECT_EQ(info.getOverallRect(), QRect(0, 0, 100, 40));    // no children expanded - overall == size
    }
}
