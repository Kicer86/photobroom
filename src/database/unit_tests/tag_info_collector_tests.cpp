
#include <gtest/gtest.h>

#include <core/base_tags.hpp>

#include <QDate>
#include <QTime>

#include "database_tools/tag_info_collector.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_photo_info.hpp"
#include "unit_tests_utils/mock_backend.hpp"
#include "unit_tests_utils/mock_db_utils.hpp"
#include "unit_tests_utils/empty_logger.hpp"


using ::testing::Invoke;
using ::testing::InvokeArgument;
using ::testing::ReturnRef;
using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;


struct Observer: QObject
{
    MOCK_METHOD1(event, void(const TagTypes &));
};


class TagInfoCollectorTest: public testing::Test
{
    public:
        TagInfoCollectorTest()
        {
            ON_CALL(database, execute(_))
                .WillByDefault(Invoke([this](std::unique_ptr<MockDatabase::ITask>&& task)
                {
                    task->run(backend);
                }));

            ON_CALL(database, backend)
                .WillByDefault(ReturnRef(backend));

            ON_CALL(database, utils)
                .WillByDefault(ReturnRef(utils));
        }

        NiceMock<MockBackend> backend;
        NiceMock<MockDatabase> database;
        NiceMock<MockUtils> utils;
};


TEST_F(TagInfoCollectorTest, Constructor)
{
    EXPECT_NO_THROW(
    {
        TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());
    });
}


TEST_F(TagInfoCollectorTest, GetWithoutDatabase)
{
    TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());

    std::vector<TagTypes> tags = BaseTags::getAll();

    for(const TagTypes& tag: tags)
    {
        const std::vector<TagValue>& values = tagInfoCollector.get(tag);

        EXPECT_EQ(values.empty(), true);
    }
}


TEST_F(TagInfoCollectorTest, LoadDataOnDatabaseSet)
{
    EXPECT_CALL(backend, listTagValues(TagTypes::Date, _))
        .WillOnce( Return(std::vector<TagValue>{QDate(0, 1, 2), QDate(1, 2, 3)}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Event, _))
        .WillOnce( Return(std::vector<TagValue>{QString("event1"), QString("event2")}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Time, _))
        .WillOnce( Return(std::vector<TagValue>{QTime(2, 3), QTime(3, 4), QTime(11, 18)}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Place, _))
        .WillOnce( Return(std::vector<TagValue>{QString("12"), QString("23")}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Rating, _))
        .WillOnce( Return(std::vector<TagValue>{5, 2, 0}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Category, _))
        .WillOnce( Return(std::vector<TagValue>{QColor(Qt::red), QColor(Qt::blue)}) );

    TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());
    tagInfoCollector.set(&database);

    const std::vector<TagValue>& dates = tagInfoCollector.get(TagTypes::Date);
    ASSERT_EQ(dates.size(), 2);
    EXPECT_EQ(dates[0].getDate(), QDate(0, 1, 2));
    EXPECT_EQ(dates[1].getDate(), QDate(1, 2, 3));

    const std::vector<TagValue>& events = tagInfoCollector.get(TagTypes::Event);
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].getString(), "event1");
    EXPECT_EQ(events[1].getString(), "event2");

    const std::vector<TagValue>& times = tagInfoCollector.get(TagTypes::Time);
    ASSERT_EQ(times.size(), 3);
    EXPECT_EQ(times[0].getTime(), QTime(2, 3));
    EXPECT_EQ(times[1].getTime(), QTime(3, 4));
    EXPECT_EQ(times[2].getTime(), QTime(11, 18));

    const std::vector<TagValue>& places = tagInfoCollector.get(TagTypes::Place);
    ASSERT_EQ(places.size(), 2);
    EXPECT_EQ(places[0].getString(), "12");
    EXPECT_EQ(places[1].getString(), "23");

    const std::vector<TagValue>& ratings = tagInfoCollector.get(TagTypes::Rating);
    ASSERT_EQ(ratings.size(), 3);
    EXPECT_EQ(ratings[0].get<int>(), 5);
    EXPECT_EQ(ratings[1].get<int>(), 2);
    EXPECT_EQ(ratings[2].get<int>(), 0);

    const std::vector<TagValue>& categories = tagInfoCollector.get(TagTypes::Category);
    ASSERT_EQ(categories.size(), 2);
    EXPECT_EQ(categories[0].get<QColor>(), Qt::red);
    EXPECT_EQ(categories[1].get<QColor>(), Qt::blue);
}


TEST_F(TagInfoCollectorTest, EmptyDatabase)
{
    EXPECT_CALL(backend, listTagValues(TagTypes::Date, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Event, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Time, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Place, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Rating, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Category, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());
    tagInfoCollector.set(&database);

    const std::vector<TagValue>& dates = tagInfoCollector.get(TagTypes::Date);
    EXPECT_TRUE(dates.empty());

    const std::vector<TagValue>& events = tagInfoCollector.get(TagTypes::Event);
    EXPECT_TRUE(events.empty());

    const std::vector<TagValue>& times = tagInfoCollector.get(TagTypes::Time);
    EXPECT_TRUE(times.empty());

    const std::vector<TagValue>& places = tagInfoCollector.get(TagTypes::Place);
    EXPECT_TRUE(places.empty());

    const std::vector<TagValue>& ratings = tagInfoCollector.get(TagTypes::Rating);
    EXPECT_TRUE(ratings.empty());

    const std::vector<TagValue>& categories = tagInfoCollector.get(TagTypes::Category);
    EXPECT_TRUE(categories.empty());
}


/*
TEST_F(TagInfoCollectorTest, ReactionOnDBChange)
{
    EXPECT_CALL(backend, listTagValues(TagTypes::Date, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Event, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Time, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Place, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Rating, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Category, _))
        .WillOnce( Return( std::vector<TagValue>()) );

    TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());
    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<NiceMock<MockPhotoInfo>>();
    Tag::TagsList tags = {
        { TagTypes::Event, TagValue(QString("event 1")) }
    };

    EXPECT_CALL(*photoInfo, getTags())
        .WillOnce(Return(tags));

    ON_CALL(*photoInfo, getID)
        .WillByDefault(Return(Photo::Id(1)));

    ON_CALL(utils, getPhotoFor(Photo::Id(1)))
        .WillByDefault(Return(photoInfo));

    emit backend.photosModified({ photoInfo->getID() });

    const std::vector<TagValue>& event = tagInfoCollector.get(TagTypes::Event);
    ASSERT_EQ(event.size(), 1);
    EXPECT_EQ(event[0].getString(), "event 1");
}


TEST_F(TagInfoCollectorTest, ObserversNotification)
{
    EXPECT_CALL(backend, listTagValues(TagTypes::Date, _))
        .WillOnce( Return(std::vector<TagValue>{QDate(0, 1, 2), QDate(1, 2, 3)}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Event, _))
        .WillOnce( Return( std::vector<TagValue>{QString("event1"), QString("event2")}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Time, _))
        .WillOnce( Return( std::vector<TagValue>{QTime(2, 3), QTime(3, 4), QTime(11, 18)}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Place, _))
        .WillOnce( Return( std::vector<TagValue>{QString("12"), QString("23")}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Rating, _))
        .WillOnce( Return( std::vector<TagValue>{5, 2, 0}) );

    EXPECT_CALL(backend, listTagValues(TagTypes::Category, _))
        .WillOnce( Return( std::vector<TagValue>{QColor(Qt::yellow), QColor(Qt::cyan)}) );

    Observer observer;

    // called 6 times by TagInfoCollector for each of TagName after database is set
    //      + 2 times after photo modification (for each tag name)
    EXPECT_CALL(observer, event(_))
        .Times(8);

    TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());
    QObject::connect(&tagInfoCollector, &TagInfoCollector::setOfValuesChanged,
                     &observer, &Observer::event);

    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<NiceMock<MockPhotoInfo>>();
    Tag::TagsList tags = {
                            { TagTypes::Time, TagValue(QTime(20, 21)) },
                            { TagTypes::Event, TagValue(QString("event123")) }
    };

    EXPECT_CALL(*photoInfo, getTags())
        .WillOnce(Return(tags));

    ON_CALL(*photoInfo, getID)
        .WillByDefault(Return(Photo::Id(1)));

    ON_CALL(utils, getPhotoFor(Photo::Id(1)))
        .WillByDefault(Return(photoInfo));

    emit backend.photosModified({ photoInfo->getID() });
}


TEST_F(TagInfoCollectorTest, ReactionOnPhotoChange)
{
    TagInfoCollector tagInfoCollector(std::make_unique<EmptyLogger>());
    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<NiceMock<MockPhotoInfo>>();
    Tag::TagsList tags = {
                            { TagTypes::Time, TagValue(QTime(2, 5)) },
                            { TagTypes::Event, TagValue(QString("event123")) }
    };

    EXPECT_CALL(*photoInfo.get(), getTags())
        .WillOnce(Return(tags));

    ON_CALL(*photoInfo, getID)
        .WillByDefault(Return(Photo::Id(1)));

    ON_CALL(utils, getPhotoFor(Photo::Id(1)))
        .WillByDefault(Return(photoInfo));

    emit backend.photosModified({ photoInfo->getID() });

    auto event = tagInfoCollector.get(TagTypes::Event);

    ASSERT_EQ(event.size(), 1);
    ASSERT_EQ(event[0].type(), Tag::ValueType::String);
    EXPECT_EQ(event[0].getString(), "event123");
}
*/
