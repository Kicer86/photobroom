
#include <gtest/gtest.h>

#include <QDate>
#include <QPixmap>

#include <core/base_tags.hpp>
#include <desktop/models/model_helpers/idx_data_manager.hpp>
#include <desktop/models/db_data_model.hpp>

#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_photo_info.hpp"
#include "unit_tests_utils/printers.hpp"
#include "test_helpers/internal_task_executor.hpp"


// TODO: tests for IdxDataManager are requiring changes in IdxDataManager (remove dependency to DBDataModel)

using ::testing::_;
using ::testing::ElementsAre;
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
    const std::vector<TagValue> dates = { QDate(2017, 05, 30) };

    EXPECT_CALL(db, listTagValues(dateTag, ElementsAre(IsEmptyFilter()), _))
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
    DBDataModel model;
    MockPhotoInfo* photoInfo = new MockPhotoInfo;
    MockPhotoInfo::Ptr photoInfoPtr(photoInfo);
    Photo::FlagValues photoFlags;
    Photo::Id photoId(1);
    Tag::TagsList photoTags;
    IdxDataManager manager(&model);

    //define expectations
    const std::vector<TagValue> empty_values;

    ON_CALL(database, notifier()).WillByDefault(Return(&notifier));
    ON_CALL(database, listTagValues(_, _, _)).WillByDefault(InvokeArgument<2>(TagNameInfo(), empty_values));

    //EXPECT_CALL(*photoInfo, getFlags()).WillRepeatedly(Return(photoFlags));
    EXPECT_CALL(*photoInfo, getID()).WillRepeatedly(Return(photoId));
    EXPECT_CALL(*photoInfo, getTags()).WillRepeatedly(Return(photoTags));

    //setup data
    manager.setDatabase(&database);

    // fetch top nodeg
    ASSERT_TRUE(manager.canFetchMore(QModelIndex()));
    manager.fetchMore(QModelIndex());

    //do test
    emit notifier.photosAdded( {photoInfoPtr} );

    //verification
    IdxNodeData* root = manager.getRoot();

    ASSERT_EQ(1, root->getChildren().size());          //one child

    ASSERT_TRUE(isNode(root->getChildren()[0].get()));
    IdxNodeData* node_child = static_cast<IdxNodeData *>(root->getChildren()[0].get());      //node child
    EXPECT_EQ(1, node_child->getChildren().size());

    IIdxData* photo_child = node_child->getChildren()[0].get();
    ASSERT_TRUE(isLeaf(photo_child));
}
*/
