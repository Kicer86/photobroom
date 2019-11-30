
#include <gmock/gmock.h>

#include "model_compositor.hpp"
#include "unit_tests_utils/mock_model_compositor_data_source.hpp"
#include "unit_tests_utils/printers.hpp"


using testing::ReturnRef;


TEST(ModelCompositorTest, emptiness)
{
    ModelCompositor model_compositor;

    EXPECT_EQ(model_compositor.rowCount(), 0);
}


TEST(ModelCompositorTest, properRowCountForSingleSource)
{
    const QStringList data = {"a", "b", "cc"};

    ModelCompositorDataSourceMock dataSourceMock;
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

    ModelCompositorDataSourceMock dataSourceMock;
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data));

    ModelCompositorDataSourceMock dataSourceMock2;
    ON_CALL(dataSourceMock2, data).WillByDefault(ReturnRef(data2));

    ModelCompositorDataSourceMock dataSourceMock3;
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

    ModelCompositorDataSourceMock dataSourceMock;
    ON_CALL(dataSourceMock, data).WillByDefault(ReturnRef(data));

    ModelCompositorDataSourceMock dataSourceMock2;
    ON_CALL(dataSourceMock2, data).WillByDefault(ReturnRef(data2));

    ModelCompositorDataSourceMock dataSourceMock3;
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
