
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/utils/model_index_utils.hpp>
#include <unit_tests_utils/printers.hpp>


using ::testing::ReturnRef;
using ::testing::_;

struct ModelIndexUtilsTest: testing::Test
{
    ModelIndexUtilsTest(): model()
    {
        for (int i = 0; i < 5; i++)
        {
            QStandardItem* top = new QStandardItem(QString("top %1").arg(i));
            for (int j = 0; j < 5; j++)
            {
                QStandardItem* child = new QStandardItem(QString("child %1").arg(j));

                top->appendRow(child);
            }

            model.appendRow(top);
        }
    }

    ~ModelIndexUtilsTest() {}

    QStandardItemModel model;
};


TEST_F(ModelIndexUtilsTest, OneLevelIncrementation)
{
    const QModelIndex parent = model.index(2, 0);
    const QModelIndex child = model.index(2, 0, parent);
    const QModelIndex sibling = model.index(3, 0, parent);
    const QModelIndex sibling_by_incrementation = utils::next(child);

    EXPECT_EQ(sibling, sibling_by_incrementation);
}


TEST_F(ModelIndexUtilsTest, OneLevelDecrementation)
{
    const QModelIndex parent = model.index(2, 0);
    const QModelIndex child = model.index(2, 0, parent);
    const QModelIndex sibling = model.index(1, 0, parent);
    const QModelIndex sibling_by_decrementation = utils::prev(child);

    EXPECT_EQ(sibling, sibling_by_decrementation);
}


TEST_F(ModelIndexUtilsTest, IncrementationOverflow)
{
    const QModelIndex parent = model.index(2, 0);
    const QModelIndex child = model.index(4, 0, parent);
    const QModelIndex sibling;
    const QModelIndex sibling_by_incrementation = utils::next(child);

    EXPECT_EQ(sibling, sibling_by_incrementation);
}


TEST_F(ModelIndexUtilsTest, DecrementationUnderflow)
{
    const QModelIndex parent = model.index(2, 0);
    const QModelIndex child = model.index(0, 0, parent);
    const QModelIndex sibling;
    const QModelIndex sibling_by_decrementation = utils::prev(child);

    EXPECT_EQ(sibling, sibling_by_decrementation);
}
