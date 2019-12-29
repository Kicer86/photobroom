
#include <gmock/gmock.h>

#include <QSignalSpy>

#include "model_compositor.hpp"
#include "unit_tests_utils/mock_model_compositor_data_source.hpp"
#include "unit_tests_utils/printers.hpp"


using testing::ReturnRef;
using testing::NiceMock;


namespace
{
    void compare_content(const QAbstractListModel& model, const QStringList& data)
    {
        const auto row_count = model.rowCount();

        QStringList read_values;
        for(int i = 0; i < row_count; i++)
        {
            const QModelIndex idx = model.index(i, 0);
            const QVariant value = idx.data();

            read_values.append(value.toString());
        }

        QStringList expectations = data;
        expectations.sort();
        read_values.sort();

        EXPECT_EQ(expectations, read_values);
    }

    std::pair<int, int> rows_range(const QSignalSpy& spy, int entry)
    {
        EXPECT_LE(entry, spy.size());

        auto insertion_data = spy.at(entry);
        EXPECT_EQ(insertion_data.size(), 3);                       // tree parameters of signal - parent, first row and last row

        return std::make_pair(insertion_data.at(1).toInt(), insertion_data.at(2).toInt());
    }

    QModelIndex rows_parent(const QSignalSpy& spy, int entry)
    {
        EXPECT_LE(entry, spy.size());

        auto insertion_data = spy.at(entry);
        EXPECT_EQ(insertion_data.size(), 3);                       // tree parameters of signal - parent, first row and last row

        return insertion_data.at(0).value<QModelIndex>();
    }

    int rows_count(const QSignalSpy& spy, int entry)
    {
        const auto range = rows_range(spy, entry);

        return range.second - range.first + 1;
    }
}


TEST(ModelCompositorTest, emptiness)
{
    ModelCompositor model_compositor;

    EXPECT_EQ(model_compositor.rowCount(), 0);
}


TEST(ModelCompositorTest, properRowCountForSingleSource)
{
    const QStringList data = {"a", "b", "cc"};

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock;
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data));

    ModelCompositor model_compositor;

    model_compositor.add(&dataSourceMock);

    EXPECT_EQ(model_compositor.rowCount(), 3);
}


TEST(ModelCompositorTest, properRowCountForManySources)
{
    const QStringList data = {"a", "b", "cc"};
    const QStringList data2 = {"q", "w", "ee", "rrrrrrrr"};
    const QStringList data3 = {"1111", "222", "3333", "5", "444444"};

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock;
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data));

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock2;
    ON_CALL(dataSourceMock2, data).WillByDefault(ReturnRef(data2));

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock3;
    ON_CALL(dataSourceMock3, data).WillByDefault(ReturnRef(data3));

    ModelCompositor model_compositor;

    model_compositor.add(&dataSourceMock);
    model_compositor.add(&dataSourceMock2);
    model_compositor.add(&dataSourceMock3);

    EXPECT_EQ(model_compositor.rowCount(), 12);
}


TEST(ModelCompositorTest, sourceDataIsReturned)
{
    // preparations
    const QStringList data = {"a", "b", "cc"};
    const QStringList data2 = {"q", "w", "ee", "rrrrrrrr"};
    const QStringList data3 = {"1111", "222", "3333", "5", "444444"};

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock;
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data));

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock2;
    ON_CALL(dataSourceMock2, data).WillByDefault(ReturnRef(data2));

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock3;
    ON_CALL(dataSourceMock3, data).WillByDefault(ReturnRef(data3));

    ModelCompositor model_compositor;

    model_compositor.add(&dataSourceMock);
    model_compositor.add(&dataSourceMock2);
    model_compositor.add(&dataSourceMock3);

    // test
    compare_content(model_compositor, data + data2 + data3);
}


TEST(ModelCompositorTest, simpleDataSourceSignalsEmission)
{
    const QStringList data = {"a", "b", "cc"};

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock;
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data));

    ModelCompositor model_compositor;
    QSignalSpy rows_removed_spy(&model_compositor, &QAbstractItemModel::rowsRemoved);
    QSignalSpy rows_inserted_spy(&model_compositor, &QAbstractItemModel::rowsInserted);

    // adding data - items should be inserted
    model_compositor.add(&dataSourceMock);
    ASSERT_EQ(rows_removed_spy.count(), 0);
    ASSERT_EQ(rows_inserted_spy.count(), 1);

    // 3 items inserted
    EXPECT_EQ(rows_inserted_spy.at(0).at(0).value<QModelIndex>(), QModelIndex());
    EXPECT_EQ(rows_inserted_spy.at(0).at(1).toInt(), 0);
    EXPECT_EQ(rows_inserted_spy.at(0).at(2).toInt(), 2);

    // changing dataset
    const QStringList data2 = {"1", "3", "5", "7"};
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data2));
    dataSourceMock.dataChanged();

    ASSERT_EQ(rows_removed_spy.count(), 1);
    ASSERT_EQ(rows_inserted_spy.count(), 2);

    // 3 items removed
    EXPECT_EQ(rows_removed_spy.at(0).at(0).value<QModelIndex>(), QModelIndex());
    EXPECT_EQ(rows_removed_spy.at(0).at(1).toInt(), 0);
    EXPECT_EQ(rows_removed_spy.at(0).at(2).toInt(), 2);

    // 4 items inserted
    EXPECT_EQ(rows_inserted_spy.at(1).at(0).value<QModelIndex>(), QModelIndex());
    EXPECT_EQ(rows_inserted_spy.at(1).at(1).toInt(), 0);
    EXPECT_EQ(rows_inserted_spy.at(1).at(2).toInt(), 3);
}


TEST(ModelCompositorTest, complexDataSourceSignalsEmission)
{
    const QStringList data1 = {"a", "b", "cc"};
    const QStringList data2 = {"1", "2", "3", "4"};
    const QStringList data3 = {"qwe", "rty", "asd", "gfd", "zxc", "bnm"};

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock1;
    ON_CALL(dataSourceMock1, data).WillByDefault(ReturnRef(data1));

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock2;
    ON_CALL(dataSourceMock2, data).WillByDefault(ReturnRef(data2));

    NiceMock<ModelCompositorDataSourceMock> dataSourceMock3;
    ON_CALL(dataSourceMock3, data).WillByDefault(ReturnRef(data3));

    ModelCompositor model_compositor;
    QSignalSpy rows_removed_spy(&model_compositor, &QAbstractItemModel::rowsRemoved);
    QSignalSpy rows_inserted_spy(&model_compositor, &QAbstractItemModel::rowsInserted);

    // adding data - items should be inserted
    model_compositor.add(&dataSourceMock1);
    model_compositor.add(&dataSourceMock2);
    model_compositor.add(&dataSourceMock3);
    ASSERT_EQ(rows_removed_spy.count(), 0);
    ASSERT_EQ(rows_inserted_spy.count(), 3);

    // validate number of inserted items
    EXPECT_EQ(rows_parent(rows_inserted_spy, 0), QModelIndex());
    EXPECT_EQ(rows_parent(rows_inserted_spy, 1), QModelIndex());
    EXPECT_EQ(rows_parent(rows_inserted_spy, 2), QModelIndex());

    const int items_inserted = rows_count(rows_inserted_spy, 0) +
                               rows_count(rows_inserted_spy, 1) +
                               rows_count(rows_inserted_spy, 2);

    EXPECT_EQ(items_inserted, 3 + 4 + 6);

    // changing dataset
    const QStringList data2_2 = {"1", "3", "5", "7", "9"};
    ON_CALL(dataSourceMock2, data).WillByDefault(ReturnRef(data2_2));
    dataSourceMock2.dataChanged();

    ASSERT_EQ(rows_removed_spy.count(), 1);
    ASSERT_EQ(rows_inserted_spy.count(), 4);

    // 4 items removed
    EXPECT_EQ(rows_parent(rows_removed_spy, 0), QModelIndex());
    EXPECT_EQ(rows_count(rows_removed_spy, 0), 4);

    // 5 items inserted
    EXPECT_EQ(rows_parent(rows_inserted_spy, 3), QModelIndex());
    EXPECT_EQ(rows_count(rows_inserted_spy, 3), 5);

    compare_content(model_compositor, data1 + data2_2 + data3);
}
