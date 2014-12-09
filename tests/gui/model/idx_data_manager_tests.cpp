
#include <gtest/gtest.h>

#include <Qt5/model_view/model_helpers/idx_data_manager.hpp>
#include <Qt5/components/photos_data_model.hpp>

#include "mock_configuration.hpp"
#include "mock_qabstractitemmodel.hpp"


TEST(IdxDataManagerShould, BeConstructable)
{
    EXPECT_NO_THROW({
        PhotosDataModel model;
        IdxDataManager manager(&model);
    });
}
