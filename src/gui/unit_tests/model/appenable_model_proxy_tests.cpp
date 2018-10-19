
#include <random>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include "widgets/tag_editor/helpers/appendable_model_proxy.hpp"


using ::testing::_;
using ::testing::ElementsAre;
using ::testing::InvokeArgument;
using ::testing::Return;


namespace
{
    std::mt19937 mt(0);

    void expectEqual(const QAbstractItemModel& m1, const QAbstractItemModel& m2)
    {
        const QModelIndex np;

        const int cols = std::min(m1.columnCount(np), m2.columnCount(np));
        const int rows = std::min(m1.rowCount(np), m2.rowCount(np));

        EXPECT_GT(cols, 0);
        EXPECT_GT(rows, 0);

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
            {
                const QModelIndex m1i = m1.index(r, c);
                const QModelIndex m2i = m2.index(r, c);

                const auto i1d = m1.itemData(m1i);
                const auto i2d = m2.itemData(m2i);

                EXPECT_EQ(i1d, i2d);
            }
    }

    QString randomString()
    {
        QString result;

        const int len = std::uniform_int_distribution<>(6, 10)(mt);
        std::uniform_int_distribution<> letters('A', 'Z');

        for(int i = 0; i < len; i++)
            result += static_cast<char>(letters(mt));

        return result;
    }

    QList<QStandardItem *> generateRandomRow(int cols)
    {
        QList<QStandardItem *> result;

        for(int i = 0; i < cols; i++)
        {
            const QString name = randomString();
            QStandardItem* item = new QStandardItem(name);

            result.append(item);
        }

        return result;
    }
}


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


TEST(AppendableModelProxyTest, DataProxying)
{
    QStandardItemModel model;

    for (int i = 0; i < 5; i++)
    {
        auto row = generateRandomRow(5);
        model.appendRow(row);
    }

    AppendableModelProxy proxy(0);
    proxy.setSourceModel(&model);

    expectEqual(model, proxy);
}

TEST(AppendableModelProxyTest, InvalidQueries)
{
    AppendableModelProxy proxy(5);

    const QModelIndex i = proxy.index(0, 0, QModelIndex());

    EXPECT_FALSE(i.isValid());     // no source model in proxy
}
