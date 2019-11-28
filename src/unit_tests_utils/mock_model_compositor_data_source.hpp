
#include <gmock/gmock.h>
#include <core/imodel_compositor_data_source.hpp>

struct ModelCompositorDataSourceMock: IModelCompositorDataSource
{
    MOCK_METHOD(const QStringList&, data, (), (const, override));
};
