
#include <gmock/gmock.h>

//#include <QDebug>
#include <QStandardItemModel>

#include <desktop/utils/model_index_utils.hpp>
#include <unit_tests_utils/printers.hpp>


using ::testing::ReturnRef;
using ::testing::_;

struct ModelIndexUtilsTest: testing::Test
{
    ModelIndexUtilsTest(): shallow_model(), deep_model()
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

        QStandardItem* top1 = new QStandardItem("top 1");
        QStandardItem* top2 = new QStandardItem("top 2");

        deep_model.appendRow(top1);
        deep_model.appendRow(top2);

        fill(deep_model, top1, 4);
        fill(deep_model, top2, 4);

        /*
        qDebug().noquote() << utils::dump(shallow_model);
        qDebug().noquote() << "";
        qDebug().noquote() << utils::dump(deep_model);
        */
    }

    ~ModelIndexUtilsTest() {}

    QStandardItemModel shallow_model;
    QStandardItemModel deep_model;

    void fill(QStandardItemModel& model, QStandardItem* parent, int level) const
    {
        if (level == 0)
            return;
        else
        {
            for (int i = 0; i < 2; i++)
            {
                QStandardItem* item = new QStandardItem( QString("child %1").arg(i) );
                parent->appendRow(item);

                fill(model, item, level - 1);
            }
        }
    };

    QModelIndex get(const QAbstractItemModel& model, const std::vector<int>& list)
    {
        QModelIndex result;
        for(int r: list)
            result = model.index(r, 0, result);

        return result;
    }
};

// Level iterations

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


// Hierarchy iterations


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


TEST_F(ModelIndexUtilsTest, JumpOutFromDeepModel)
{
    const QModelIndex item1 = get(deep_model, {0, 1, 1, 1, 1});
    const QModelIndex next1 = get(deep_model, {1});
    const QModelIndex item2 = get(deep_model, {1, 1, 0, 1, 1});
    const QModelIndex next2 = get(deep_model, {1, 1, 1});
    const QModelIndex item3 = get(deep_model, {1, 1, 1, 1, 1});
    const QModelIndex next3;

    const QModelIndex calculated_next1 = utils::step_in_next(item1);
    const QModelIndex calculated_next2 = utils::step_in_next(item2);
    const QModelIndex calculated_next3 = utils::step_in_next(item3);

    EXPECT_EQ(next1, calculated_next1);
    EXPECT_EQ(next2, calculated_next2);
    EXPECT_EQ(next3, calculated_next3);
}


TEST_F(ModelIndexUtilsTest, GoToPreviousSiblingThroughChildren)
{
    const QModelIndex top1 = shallow_model.index(3, 0);
    const QModelIndex top2 = shallow_model.index(2, 0);
    QModelIndex index_iter = utils::step_in_prev(top1);

    for (int i = shallow_model.rowCount(top1) - 1; i >= 0 ; i--)
    {
        const QModelIndex child = shallow_model.index(i, 0, top2);

        EXPECT_EQ(child, index_iter);

        index_iter = utils::step_in_prev(index_iter);
    }

    EXPECT_EQ(top2, index_iter);
}


TEST_F(ModelIndexUtilsTest, JumpIntoDeepModel)
{
    const QModelIndex item1 = get(deep_model, {1});
    const QModelIndex prev1 = get(deep_model, {0, 1, 1, 1, 1});
    const QModelIndex item2 = get(deep_model, {1, 1, 1});
    const QModelIndex prev2 = get(deep_model, {1, 1, 0, 1, 1});
    const QModelIndex item3 = get(deep_model, {0});
    const QModelIndex prev3;

    const QModelIndex calculated_prev1 = utils::step_in_prev(item1);
    const QModelIndex calculated_prev2 = utils::step_in_prev(item2);
    const QModelIndex calculated_prev3 = utils::step_in_prev(item3);

    EXPECT_EQ(prev1, calculated_prev1);
    EXPECT_EQ(prev2, calculated_prev2);
    EXPECT_EQ(prev3, calculated_prev3);
}
