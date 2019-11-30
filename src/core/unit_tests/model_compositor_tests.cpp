
#include <gmock/gmock.h>

#include "model_compositor.hpp"
#include "unit_tests_utils/mock_model_compositor_data_source.hpp"


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
