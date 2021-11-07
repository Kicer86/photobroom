
#include <gmock/gmock.h>
#include <QSignalSpy>

#include <database/database_tools/json_to_backend.hpp>
#include <database/backends/memory_backend/memory_backend.hpp>
#include "desktop/models/flat_model.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_backend.hpp"
#include "unit_tests_utils/mock_photo_operator.hpp"
#include "unit_tests_utils/sample_db.json.hpp"


using testing::_;
using testing::Invoke;
using testing::InvokeArgument;
using testing::Return;
using testing::ReturnRef;
using testing::NiceMock;


class FlatModelTest: public testing::Test
{
    public:
        FlatModel model;
        NiceMock<MockDatabase> db;
        NiceMock<MockBackend> backend;
        NiceMock<PhotoOperatorMock> photoOperator;

        FlatModelTest()
        {
            ON_CALL(backend, photoOperator()).WillByDefault(ReturnRef(photoOperator));

            ON_CALL(db, execute(_)).WillByDefault(Invoke([this](std::unique_ptr<Database::IDatabase::ITask>&& task)
            {
                task->run(backend);
            }));

            ON_CALL(db, backend).WillByDefault(ReturnRef(backend));
        }
};


TEST_F(FlatModelTest, resetAfterDBSet)
{
    QSignalSpy model_about_to_be_reset(&model, &FlatModel::modelAboutToBeReset);
    QSignalSpy model_reset(&model, &FlatModel::modelReset);

    model.setDatabase(nullptr);
    EXPECT_EQ(model_about_to_be_reset.count(), 1);
    EXPECT_EQ(model_reset.count(), 1);

    model.setDatabase(&db);
    EXPECT_EQ(model_about_to_be_reset.count(), 2);
    EXPECT_EQ(model_reset.count(), 2);
}


TEST_F(FlatModelTest, insertNotificationAfterDBSet)
{
    auto photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(photos_set));

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);

    EXPECT_EQ(model_about_to_be_inserted.count(), 1);
    EXPECT_EQ(model_inserted.count(), 1);

    // we expect 3 items were inserted on position 0, 1 and 2
    EXPECT_EQ(model_inserted.at(0).at(1).toInt(), 0);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(photos_set, model.photos());
}


TEST_F(FlatModelTest, dataAppended)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))   // first call after db set
        .WillOnce(Return(final_photos_set));    // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 2 insertions - one after db set, second after filters set
    EXPECT_EQ(model_about_to_be_inserted.count(), 2);
    EXPECT_EQ(model_inserted.count(), 2);

    // we expect 1 item to be inserted on position 3
    EXPECT_EQ(model_inserted.at(1).at(1).toInt(), 3);       // second(1) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(1).at(2).toInt(), 3);       // second(1) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataPrepended)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(4), Photo::Id(1), Photo::Id(2), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))       // first call after db set
        .WillOnce(Return(final_photos_set));        // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 2 insertions - one after db set, second after filters set
    EXPECT_EQ(model_about_to_be_inserted.count(), 2);
    EXPECT_EQ(model_inserted.count(), 2);

    // we expect 1 item to be inserted on position 0
    EXPECT_EQ(model_inserted.at(1).at(1).toInt(), 0);       // second(1) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(1).at(2).toInt(), 0);       // second(1) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataInserting)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(4), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))   // first call after db set
        .WillOnce(Return(final_photos_set));    // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 2 insertions - one after db set, second after filters set
    EXPECT_EQ(model_about_to_be_inserted.count(), 2);
    EXPECT_EQ(model_inserted.count(), 2);

    // we expect 1 item to be inserted on position 0
    EXPECT_EQ(model_inserted.at(1).at(1).toInt(), 2);       // second(1) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(1).at(2).toInt(), 2);       // second(1) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedAtFront)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(2), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 0
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 0);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 0);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedAtFrontByBackend)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(2), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set));                // first call after db set

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    backend.photosRemoved( {Photo::Id(1)} );                  // emit notification about photo removal

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 0
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 0);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 0);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedAtBack)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 2
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 2);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedAtBackByBackend)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set));                // first call after db set

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    backend.photosRemoved( {Photo::Id(3)} );                  // emit notification about photo removal

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 2
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 2);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedInTheMiddle)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 1
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 1);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 1);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedInTheMiddleByBackend)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set));                // first call after db set

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    backend.photosRemoved( {Photo::Id(2)} );                  // emit notification about photo removal

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 1
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 1);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 1);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, removingAll)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 3 items to be removed
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 0);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, removingAllByBackend)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set));                // first call after db set

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    backend.photosRemoved( {Photo::Id(1), Photo::Id(2), Photo::Id(3)} ); // emit notification about photo removal

    // 1 removal expected
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);

    // we expect 3 items to be removed
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 0);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataReplacementInTheMiddle)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(4), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))               // first call after db set
        .WillOnce(Return(final_photos_set));                // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);
    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    //
    // 1 removal expected and 2 insertions - one after db set, second after filters set
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);
    EXPECT_EQ(model_about_to_be_inserted.count(), 2);
    EXPECT_EQ(model_inserted.count(), 2);

    // we expect one item to be removed and one to be insered
    // There are two scenarios: either we insert first, then remove
    // or oposite. Depending on order positions may be swapped.
    // Current implementation will insert before removal
    EXPECT_EQ(model_inserted.at(1).at(1).toInt(), 1);       // second(1) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(1).at(2).toInt(), 1);       // second(1) instance of signal, third(2) argument   (last)
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 2);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataReplacementAtFront)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(4), Photo::Id(2), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))               // first call after db set
        .WillOnce(Return(final_photos_set));                // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);
    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    //
    // 1 removal expected and 2 insertions - one after db set, second after filters set
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);
    EXPECT_EQ(model_about_to_be_inserted.count(), 2);
    EXPECT_EQ(model_inserted.count(), 2);

    // we expect one item to be removed and one to be insered
    // There are two scenarios: either we insert first, then remove
    // or oposite. Depending on order positions may be swapped.
    // Current implementation will insert before removal
    EXPECT_EQ(model_inserted.at(1).at(1).toInt(), 0);       // second(1) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(1).at(2).toInt(), 0);       // second(1) instance of signal, third(2) argument   (last)
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 1);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 1);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataReplacementAtBack)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(4)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);
    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    //
    // 1 removal expected and 2 insertions - one after db set, second after filters set
    ASSERT_EQ(model_about_to_be_removed.count(), 1);
    ASSERT_EQ(model_removed.count(), 1);
    EXPECT_EQ(model_about_to_be_inserted.count(), 2);
    EXPECT_EQ(model_inserted.count(), 2);

    // we expect one item to be removed and one to be insered
    // There are two scenarios: either we insert first, then remove
    // or oposite. Depending on order positions may be swapped.
    // Current implementation will remove before insertion
    EXPECT_EQ(model_inserted.at(1).at(1).toInt(), 2);       // second(1) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_inserted.at(1).at(2).toInt(), 2);       // second(1) instance of signal, third(2) argument   (last)
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 2);        // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);        // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, complexChanges)
{
    const auto initial_photos_set = std::vector<Photo::Id>{                            Photo::Id(11), Photo::Id(12), Photo::Id(13), Photo::Id(14),                               Photo::Id(15), Photo::Id(16), Photo::Id(17), Photo::Id(18)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(21), Photo::Id(22),                                              Photo::Id(14), Photo::Id(23), Photo::Id(24), Photo::Id(15), Photo::Id(16),                              Photo::Id(25), Photo::Id(26)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, complexChangesByBackend)
{
    const auto initial_photos_set = std::vector<Photo::Id>{                            Photo::Id(11), Photo::Id(12), Photo::Id(13), Photo::Id(14),                               Photo::Id(15), Photo::Id(16), Photo::Id(17), Photo::Id(18)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(21), Photo::Id(22),                                              Photo::Id(14), Photo::Id(23), Photo::Id(24), Photo::Id(15), Photo::Id(16),                              Photo::Id(25), Photo::Id(26)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after backend.photosAdded. TODO: this is not nice that test is aware of implementation

    model.setDatabase(&db);
    backend.photosRemoved( {Photo::Id(11), Photo::Id(12), Photo::Id(13), Photo::Id(17), Photo::Id(18)} );
    backend.photosAdded( {Photo::Id(21), Photo::Id(22), Photo::Id(23), Photo::Id(24), Photo::Id(25), Photo::Id(26)} );

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, PhotosReorder)
{
    const auto initial_photos_set = std::vector<Photo::Id>{ Photo::Id(1), Photo::Id(2), Photo::Id(3) };
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(3), Photo::Id(2)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))
        .WillOnce(Return(final_photos_set));

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    ON_CALL(photoOperator, onPhotos(_, _))
        .WillByDefault(Return(final_photos_set));

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, DataChange)
{
    const auto initial_photos_set = std::vector<Photo::Id>{ Photo::Id(1), Photo::Id(2), Photo::Id(3), Photo::Id(4) };

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set));

    QSignalSpy model_data_changed(&model, &FlatModel::dataChanged);

    model.setDatabase(&db);
    backend.photosModified({Photo::Id(2), Photo::Id(3)});

    ASSERT_EQ(model_data_changed.count(), 1);
    EXPECT_EQ(model_data_changed.at(0).at(0).toModelIndex(), model.index(1, 0, {}));       // we expect that rows from 1 to 2
    EXPECT_EQ(model_data_changed.at(0).at(1).toModelIndex(), model.index(2, 0, {}));       // have changed
}


TEST_F(FlatModelTest, accessToPhotoPathByItemIndex)
{
    NiceMock<MockDatabase> db;
    Database::MemoryBackend backend;
    Database::JsonToBackend jsonReader(backend);

    jsonReader.append(SampleDB::db1);

    ON_CALL(db, execute(_)).WillByDefault(Invoke([&backend](auto&& task)
    {
        task->run(backend);
    }));

    ON_CALL(db, backend).WillByDefault(ReturnRef(backend));

    model.setDatabase(&db);
    model.setFilter({});       // setting filters should update set of photos

    const auto path = model.getPhotoPath(1);
    EXPECT_EQ(path, QUrl::fromLocalFile("/some/path2.jpeg"));
}


TEST_F(FlatModelTest, includeIgnoredPhotosIfTheyMatchNow)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};

    EXPECT_CALL(photoOperator, onPhotos(_, _))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // called after photo modification. WARNING: this is implementation aware

    model.setDatabase(&db);
    EXPECT_EQ(initial_photos_set, model.photos());

    backend.photosModified({Photo::Id(3)});                   // photo with id = 3 changed and now matches filters
    EXPECT_EQ(final_photos_set, model.photos());
}
