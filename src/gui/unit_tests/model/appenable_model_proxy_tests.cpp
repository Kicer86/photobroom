
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include "widgets/tag_editor/helpers/appendable_model_proxy.hpp"


using ::testing::_;
using ::testing::ElementsAre;
using ::testing::InvokeArgument;
using ::testing::Return;


TEST(AppendableModelProxyTest, IsConstructable)
{
    EXPECT_NO_THROW({
        AppendableModelProxy proxy(0, nullptr);
    });
}


TEST(AppendableModelProxyTest, NoModelNoData)
{
    AppendableModelProxy proxy(2, nullptr);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 0);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 0);
}


TEST(AppendableModelProxyTest, EmptyModelAppendableRow)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(2, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 2);
}
