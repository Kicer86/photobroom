
#include <gtest/gtest.h>

#include <QPixmap>

#include <core/base_tags.hpp>
#include <Qt5/model_view/model_helpers/idx_data_manager.hpp>
#include <Qt5/components/photos_data_model.hpp>

#include "test_helpers/mock_database.hpp"
#include "test_helpers/internal_task_executor.hpp"
#include "test_helpers/mock_photo_info.hpp"


namespace
{
    struct DatabaseNotifier: Database::ADatabaseSignals
    {
    };

    struct PhotoInfo: IPhotoInfo
    {

    };
}


TEST(IdxDataManagerShould, BeConstructable)
{
    EXPECT_NO_THROW({
        PhotosDataModel model;
        IdxDataManager manager(&model);
    });
}


TEST(IdxDataManagerShould, AddUniversalNodeOnTopWhenPhotoDoesntMatchOtherTopNodes)
{
    using ::testing::Return;

    //construct objects
    DatabaseNotifier notifier;
    MockDatabase database;
    InternalTaskExecutor executor;

    Hierarchy hierarchy;
    hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending } };
    
    PhotosDataModel model;
    model.setDatabase(&database);
    model.setHierarchy(hierarchy);
    model.set(&executor);

    //prepare data
    MockPhotoInfo::Ptr photoInfo = std::make_shared<MockPhotoInfo>();
    
    //define expectations
    EXPECT_CALL(database, notifier()).WillRepeatedly(Return(&notifier));

    //do test
    notifier.photoAdded(photoInfo);
}
