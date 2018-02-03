
#include <gmock/gmock.h>

#include <QStandardItemModel>

#include <desktop/utils/model_index_utils.hpp>
#include <unit_tests_utils/printers.hpp>


using ::testing::ReturnRef;
using ::testing::_;

struct ModelIndexUtilsTest: testing::Test
{
    ModelIndexUtilsTest(): shallow_model()
    {
        for (int i = 0; i < 5; i++)
        {
            QStandardItem* top = new QStandardItem(QString("top %1").arg(i));
            for (int j = 0; j < 5; j++)
            {
                QStandardItem* child = new QStandardItem(QString("child %1").arg(j));

                top->appendRow(child);
            }

            shallow_model.appendRow(top);
        }
    }

    ~ModelIndexUtilsTest() {}

    QStandardItemModel shallow_model;
};


TEST_F(ModelIndexUtilsTest, OneLevelIncrementation)
{
    const QModelIndex parent = shallow_model.index(2, 0);
    const QModelIndex child = shallow_model.index(2, 0, parent);
    const QModelIndex sibling = shallow_model.index(3, 0, parent);
    const QModelIndex sibling_by_incrementation = utils::next(child);

    EXPECT_EQ(sibling, sibling_by_incrementation);
}


TEST_F(ModelIndexUtilsTest, OneLevelDecrementation)
{
    const QModelIndex parent = shallow_model.index(2, 0);
    const QModelIndex child = shallow_model.index(2, 0, parent);
    const QModelIndex sibling = shallow_model.index(1, 0, parent);
    const QModelIndex sibling_by_decrementation = utils::prev(child);

    EXPECT_EQ(sibling, sibling_by_decrementation);
}


TEST_F(ModelIndexUtilsTest, IncrementationOverflow)
{
    const QModelIndex parent = shallow_model.index(2, 0);
    const QModelIndex child = shallow_model.index(4, 0, parent);
    const QModelIndex sibling;
    const QModelIndex sibling_by_incrementation = utils::next(child);

    EXPECT_EQ(sibling, sibling_by_incrementation);
}


TEST_F(ModelIndexUtilsTest, DecrementationUnderflow)
{
    const QModelIndex parent = shallow_model.index(2, 0);
    const QModelIndex child = shallow_model.index(0, 0, parent);
    const QModelIndex sibling;
    const QModelIndex sibling_by_decrementation = utils::prev(child);

    EXPECT_EQ(sibling, sibling_by_decrementation);
}


TEST_F(ModelIndexUtilsTest, GoToNextSiblingThroughChildren)
{
    const QModelIndex top1 = shallow_model.index(2, 0);
    QModelIndex index_iter = utils::step_in_next(top1);

    for (int i = 0; i < shallow_model.rowCount(top1); i++)
    {
        const QModelIndex child = shallow_model.index(i, 0, top1);

        EXPECT_EQ(child, index_iter);

        index_iter = utils::step_in_next(index_iter);
    }

    const QModelIndex top2 = shallow_model.index(3, 0);
    EXPECT_EQ(top2, index_iter);
}

