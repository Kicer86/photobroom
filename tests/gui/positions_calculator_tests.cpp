
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

