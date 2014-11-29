
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include <configuration/constants.hpp>

#include "positions_calculator.hpp"
#include "data.hpp"

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

    MockConfiguration config;
    MockQAbstractItemModel model;

    auto createIndex = boost::bind(&MockQAbstractItemModel::createIndex, boost::ref(model), _1, _2, nullptr);

    EXPECT_CALL(config, findEntry(Configuration::BasicKeys::thumbnailWidth, _)).Times(1).WillOnce(Return("20"));
    EXPECT_CALL(model, rowCount(top)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(model, index(0, 0, top))
        .Times(1)
        .WillOnce(Invoke(createIndex));

    QModelIndex child = model.index(0, 0, top);

    Data data;
    data.m_configuration = &config;
    data.get(child);                         //introduce child item

    PositionsCalculator calculator(&model, &data, 100);
    calculator.updateItems();

    const ModelIndexInfo info = data.get(top);

    EXPECT_EQ(info.getRect(), QRect());
    EXPECT_EQ(info.getOverallRect(), QRect());
}
