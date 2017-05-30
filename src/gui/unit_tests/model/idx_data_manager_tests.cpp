
#include <gtest/gtest.h>

#include <QDate>
#include <QPixmap>

#include <core/base_tags.hpp>
#include <desktop/models/model_helpers/idx_data_manager.hpp>
#include <desktop/models/db_data_model.hpp>

#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_photo_info.hpp"
#include "test_helpers/internal_task_executor.hpp"


// TODO: tests for IdxDataManager are requiring changes in IdxDataManager (remove dependency to DBDataModel)

using ::testing::_;
using ::testing::Contains;
using ::testing::InvokeArgument;
using ::testing::Return;

MATCHER(IsEmptyFilter, "")
{
    Database::IFilter* src = arg.get();
    Database::EmptyFilter* empty_filter = dynamic_cast<Database::EmptyFilter *>(src);

    return empty_filter != nullptr;
}

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
        DBDataModel model;
        IdxDataManager manager(&model);
    });
}


TEST(IdxDataManagerShould, CleanupOnNodeIdxDestruction)
{
    MockDatabase db;

    const TagNameInfo dateTag(BaseTagsList::Date);
    const std::deque<TagValue> dates = { QDate(2017, 05, 30) };

    EXPECT_CALL(db, notifier())
        .WillRepeatedly(Return(nullptr));

    EXPECT_CALL(db, listTagValues(dateTag, Contains(IsEmptyFilter()), _))
        .WillOnce(InvokeArgument<2>(dateTag, dates));

    // filter, callback
    EXPECT_CALL(db, countPhotos(_, _));

    DBDataModel model;
    model.setDatabase(&db);
    IdxDataManager manager(&model);

    ASSERT_TRUE(model.canFetchMore(QModelIndex()));
    model.fetchMore(QModelIndex());
}

/*
TEST(IdxDataManagerShould, AddUniversalNodeOnTopWhenPhotoDoesntMatchOtherTopNodes)
{
    using ::testing::Return;
    using ::testing::ReturnRef;

    //construct objects
    DatabaseNotifier notifier;
    MockDatabase database;
    InternalTaskExecutor executor;
    Hierarchy hierarchy;
    PhotosDataModel model;
    MockPhotoInfo* photoInfo = new MockPhotoInfo;
    MockPhotoInfo::Ptr photoInfoPtr(photoInfo);
    IPhotoInfo::Flags photoFlags;
    IPhotoInfo::Id photoId(1);
    Tag::TagsList photoTags;
    IdxDataManager manager(&model);

    //define expectations
    EXPECT_CALL(database, notifier()).WillRepeatedly(Return(&notifier));
    EXPECT_CALL(*photoInfo, getFlags()).WillRepeatedly(Return(photoFlags));
    EXPECT_CALL(*photoInfo, getID()).WillRepeatedly(Return(photoId));
    EXPECT_CALL(*photoInfo, getTags()).WillRepeatedly(ReturnRef(photoTags));

    //setup data
    hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending } };
    model.setDatabase(&database);
    model.setHierarchy(hierarchy);
    model.set(&executor);

    //do test
    emit notifier.photoAdded(photoInfoPtr);

    //verification
    IdxData* root = manager.getRoot();

    EXPECT_EQ(1, root->m_children.size());          //one child

    IdxData* node_child = root->m_children[0];      //node child
    EXPECT_EQ(true, node_child->isNode());
    EXPECT_EQ(1, node_child->m_children.size());

    IdxData* photo_child = node_child->m_children[0];
    EXPECT_EQ(true, photo_child->isPhoto());
    EXPECT_EQ(true, photo_child->m_children.empty());
}
*/
