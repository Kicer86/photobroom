
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


TEST(AppendableModelProxyTest, NoModel_NoData)
{
    AppendableModelProxy proxy(2, nullptr);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 0);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 0);
}


TEST(AppendableModelProxyTest, EmptyModel_AppendableRow)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(2, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 2);
}


TEST(AppendableModelProxyTest, EmptyModel_ExtraRowDisabled)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(2, nullptr);
    proxy.enableAppending(false);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 0);
}


TEST(AppendableModelProxyTest, ReactionOnModelGrowthWhenColumnsMatch)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(3, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 5);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);
}


TEST(AppendableModelProxyTest, ReactionOnModelShrinkWhenColumnsMatch)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(3, nullptr);
    proxy.setSourceModel(&model);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 5);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(0, 1);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 4);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(0, 1);
    model.removeRows(0, 1);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(0, 1);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);
}


TEST(AppendableModelProxyTest, ReactionOnPrefilledModelLoadWhenColumnsMatch)
{
    QStandardItemModel model;
    AppendableModelProxy proxy(3, nullptr);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 5);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);
}


TEST(AppendableModelProxyTest, ReactionOnModelGrowthWhenLessColumns)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(5, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 5);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);
}


TEST(AppendableModelProxyTest, ReactionOnModelGrowthWhenMoreColumns)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(5, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 5);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem,
                     new QStandardItem, new QStandardItem, new QStandardItem,
                     new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 9);
}


TEST(AppendableModelProxyTest, GrowthWithChangingAdditionalRow)
{
    QStandardItemModel model;

    AppendableModelProxy proxy(5, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 5);

    proxy.enableAppending(false);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 0);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(true);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(false);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(true);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 3);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);
}


TEST(AppendableModelProxyTest, ShrinkWithChangingAdditionalRow)
{
    QStandardItemModel model;
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});
    model.appendRow({new QStandardItem, new QStandardItem, new QStandardItem});

    AppendableModelProxy proxy(5, nullptr);
    proxy.setSourceModel(&model);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 6);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(false);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 5);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(2, 1);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 4);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(true);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 5);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(false);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 4);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(0, 1);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 3);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(true);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 4);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(1, 1);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 3);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    proxy.enableAppending(false);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 2);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);

    model.removeRows(0, 2);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 0);

    proxy.enableAppending(true);

    EXPECT_EQ(proxy.rowCount(QModelIndex()), 1);
    EXPECT_EQ(proxy.columnCount(QModelIndex()), 3);
}
