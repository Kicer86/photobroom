
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
    const auto row_count = model_compositor.rowCount();

    QStringList read_values;
    for(int i = 0; i < row_count; i++)
    {
        const QModelIndex idx = model_compositor.index(i, 0);
        const QVariant value = idx.data();

        read_values.append(value.toString());
    }

    QStringList combined_expectations = data + data2 + data3;

    combined_expectations.sort();
    read_values.sort();

    EXPECT_EQ(combined_expectations, read_values);
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
