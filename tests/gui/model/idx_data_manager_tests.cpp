
#include <gtest/gtest.h>

#include <Qt5/model_view/model_helpers/idx_data_manager.hpp>
#include <Qt5/components/photos_data_model.hpp>

#include "mock_database.hpp"


TEST(IdxDataManagerShould, BeConstructable)
{
    EXPECT_NO_THROW({
        PhotosDataModel model;
        IdxDataManager manager(&model);
    });
}


TEST(IdxDataManagerShould, AddUniversalNodeOnTopWhenPhotoDoesntMatchOtherTopNodes)
{
    MockDatabase database;
    
    PhotosDataModel model;
    model.setDatabase(&database);
}

