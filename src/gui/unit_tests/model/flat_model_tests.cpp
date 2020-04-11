
#include <gmock/gmock.h>
#include <QSignalSpy>

#include "desktop/models/flat_model.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_backend.hpp"


using testing::_;
using testing::Invoke;
using testing::Return;
using testing::NiceMock;


class FlatModelTest: public testing::Test
{
    public:
        FlatModel model;
        NiceMock<MockDatabase> db;
        NiceMock<MockBackend> backend;

        FlatModelTest()
        {
            ON_CALL(db, execute(_)).WillByDefault(Invoke([this](std::unique_ptr<Database::IDatabase::ITask>&& task)
            {
                task->run(&backend);
            }));
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

    EXPECT_CALL(backend, getPhotos(_))
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

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))   // first call after db set
        .WillOnce(Return(final_photos_set));    // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

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

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))       // first call after db set
        .WillOnce(Return(final_photos_set));        // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

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

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))   // first call after db set
        .WillOnce(Return(final_photos_set));    // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

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

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

    // 1 removal expected
    EXPECT_EQ(model_about_to_be_removed.count(), 1);
    EXPECT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 0
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 0);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 0);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedAtBack)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2)};

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

    // 1 removal expected
    EXPECT_EQ(model_about_to_be_removed.count(), 1);
    EXPECT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 2
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 2);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 2);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataRemovedInTheMiddle)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(3)};

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

    // 1 removal expected
    EXPECT_EQ(model_about_to_be_removed.count(), 1);
    EXPECT_EQ(model_removed.count(), 1);

    // we expect 1 item to be removed on position 1
    EXPECT_EQ(model_removed.at(0).at(1).toInt(), 1);       // first(0) instance of signal, second(1) argument  (first)
    EXPECT_EQ(model_removed.at(0).at(2).toInt(), 1);       // first(0) instance of signal, third(2) argument   (last)

    EXPECT_EQ(final_photos_set, model.photos());
}


TEST_F(FlatModelTest, dataReplacementInTheMiddle)
{
    const auto initial_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(2), Photo::Id(3)};
    const auto final_photos_set = std::vector<Photo::Id>{Photo::Id(1), Photo::Id(4), Photo::Id(3)};

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))               // first call after db set
        .WillOnce(Return(final_photos_set));                // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);
    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

    //
    // 1 removal expected and 2 insertions - one after db set, second after filters set
    EXPECT_EQ(model_about_to_be_removed.count(), 1);
    EXPECT_EQ(model_removed.count(), 1);
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

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))               // first call after db set
        .WillOnce(Return(final_photos_set));                // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);
    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

    //
    // 1 removal expected and 2 insertions - one after db set, second after filters set
    EXPECT_EQ(model_about_to_be_removed.count(), 1);
    EXPECT_EQ(model_removed.count(), 1);
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

    EXPECT_CALL(backend, getPhotos(_))
        .WillOnce(Return(initial_photos_set))                 // first call after db set
        .WillOnce(Return(final_photos_set));                  // second call after setting filters

    QSignalSpy model_about_to_be_inserted(&model, &FlatModel::rowsAboutToBeInserted);
    QSignalSpy model_inserted(&model, &FlatModel::rowsInserted);
    QSignalSpy model_about_to_be_removed(&model, &FlatModel::rowsAboutToBeRemoved);
    QSignalSpy model_removed(&model, &FlatModel::rowsRemoved);

    model.setDatabase(&db);
    model.setFilters({});       // setting filters should update set of photos

    //
    // 1 removal expected and 2 insertions - one after db set, second after filters set
    EXPECT_EQ(model_about_to_be_removed.count(), 1);
    EXPECT_EQ(model_removed.count(), 1);
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
