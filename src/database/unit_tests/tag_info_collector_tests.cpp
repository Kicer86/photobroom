
#include <gtest/gtest.h>

#include <core/base_tags.hpp>

#include <QDate>
#include <QTime>

#include "database_tools/tag_info_collector.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_photo_info.hpp"
#include "unit_tests_utils/mock_backend.hpp"
#include "unit_tests_utils/mock_db_utils.hpp"


using ::testing::InvokeArgument;
using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;


struct Observer: QObject
{
    MOCK_METHOD1(event, void(const TagNameInfo &));
};


TEST(TagInfoCollectorTest, Constructor)
{
    EXPECT_NO_THROW(
    {
        TagInfoCollector tagInfoCollector;
    });
}


TEST(TagInfoCollectorTest, GetWithoutDatabase)
{
    TagInfoCollector tagInfoCollector;

    std::vector<BaseTagsList> tags = BaseTags::getAll();

    for(const BaseTagsList& tag: tags)
    {
        const TagNameInfo info(tag);
        const std::vector<TagValue>& values = tagInfoCollector.get(info);

        EXPECT_EQ(values.empty(), true);
    }
}


TEST(TagInfoCollectorTest, LoadDataOnDatabaseSet)
{
    MockBackend backend;
    NiceMock<MockDatabase> database;

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>{QDate(0, 1, 2), QDate(1, 2, 3)}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>{QString("event1"), QString("event2")}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>{QTime(2, 3), QTime(3, 4), QTime(11, 18)}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>{QString("12"), QString("23")}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Rating), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Rating), std::vector<TagValue>{5.0, 1.5, 0.0}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Category), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Category), std::vector<TagValue>{0x110055llu, 0xff00ffllu}) );

    ON_CALL(database, backend)
        .WillByDefault(Return(&backend));

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    const std::vector<TagValue>& dates = tagInfoCollector.get( TagNameInfo(BaseTagsList::Date) );
    ASSERT_EQ(dates.size(), 2);
    EXPECT_EQ(dates[0].getDate(), QDate(0, 1, 2));
    EXPECT_EQ(dates[1].getDate(), QDate(1, 2, 3));

    const std::vector<TagValue>& events = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].getString(), "event1");
    EXPECT_EQ(events[1].getString(), "event2");

    const std::vector<TagValue>& times = tagInfoCollector.get( TagNameInfo(BaseTagsList::Time) );
    ASSERT_EQ(times.size(), 3);
    EXPECT_EQ(times[0].getTime(), QTime(2, 3));
    EXPECT_EQ(times[1].getTime(), QTime(3, 4));
    EXPECT_EQ(times[2].getTime(), QTime(11, 18));

    const std::vector<TagValue>& places = tagInfoCollector.get( TagNameInfo(BaseTagsList::Place) );
    ASSERT_EQ(places.size(), 2);
    EXPECT_EQ(places[0].getString(), "12");
    EXPECT_EQ(places[1].getString(), "23");

    const std::vector<TagValue>& ratings = tagInfoCollector.get( TagNameInfo(BaseTagsList::Rating) );
    ASSERT_EQ(ratings.size(), 3);
    EXPECT_EQ(ratings[0].get<double>(), 5.0);
    EXPECT_EQ(ratings[1].get<double>(), 1.5);
    EXPECT_EQ(ratings[2].get<double>(), 0.0);

    const std::vector<TagValue>& categories = tagInfoCollector.get( TagNameInfo(BaseTagsList::Category) );
    ASSERT_EQ(categories.size(), 2);
    EXPECT_EQ(categories[0].get<quint64>(), 0x110055);
    EXPECT_EQ(categories[1].get<quint64>(), 0xff00ff);
}


TEST(TagInfoCollectorTest, EmptyDatabase)
{
    MockBackend backend;
    NiceMock<MockDatabase> database;

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Rating), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Rating), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Category), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Category), std::vector<TagValue>()) );

    ON_CALL(database, backend)
        .WillByDefault(Return(&backend));

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    const std::vector<TagValue>& dates = tagInfoCollector.get( TagNameInfo(BaseTagsList::Date) );
    EXPECT_TRUE(dates.empty());

    const std::vector<TagValue>& events = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );
    EXPECT_TRUE(events.empty());

    const std::vector<TagValue>& times = tagInfoCollector.get( TagNameInfo(BaseTagsList::Time) );
    EXPECT_TRUE(times.empty());

    const std::vector<TagValue>& places = tagInfoCollector.get( TagNameInfo(BaseTagsList::Place) );
    EXPECT_TRUE(places.empty());

    const std::vector<TagValue>& ratings = tagInfoCollector.get( TagNameInfo(BaseTagsList::Rating) );
    EXPECT_TRUE(ratings.empty());

    const std::vector<TagValue>& categories = tagInfoCollector.get( TagNameInfo(BaseTagsList::Category) );
    EXPECT_TRUE(categories.empty());
}


TEST(TagInfoCollectorTest, ReactionOnDBChange)
{
    NiceMock<MockUtils> utils;
    MockBackend backend;
    NiceMock<MockDatabase> database;

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Rating), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Rating), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Category), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Category), std::vector<TagValue>()) );

    ON_CALL(database, backend)
        .WillByDefault(Return(&backend));

    ON_CALL(database, utils)
        .WillByDefault(Return(&utils));

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<NiceMock<MockPhotoInfo>>();
    Tag::TagsList tags = {
        { TagNameInfo(BaseTagsList::_People), TagValue(QString("person123")) }
    };

    EXPECT_CALL(*photoInfo, getTags())
        .WillOnce(Return(tags));

    ON_CALL(*photoInfo, getID)
        .WillByDefault(Return(Photo::Id(1)));

    ON_CALL(utils, getPhotoFor(Photo::Id(1)))
        .WillByDefault(Return(photoInfo));

    emit backend.photoModified(photoInfo->getID());

    const std::vector<TagValue>& people = tagInfoCollector.get( TagNameInfo(BaseTagsList::_People) );
    ASSERT_EQ(people.size(), 1);
    EXPECT_EQ(people[0].getString(), "person123");
}


TEST(TagInfoCollectorTest, ObserversNotification)
{
    NiceMock<MockUtils> utils;
    MockBackend backend;
    NiceMock<MockDatabase> database;

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>{QDate(0, 1, 2), QDate(1, 2, 3)}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>{QString("event1"), QString("event2")}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>{QTime(2, 3), QTime(3, 4), QTime(11, 18)}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>{QString("12"), QString("23")}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Rating), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Rating), std::vector<TagValue>{5.0, 1.5, 0.0}) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Category), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Category), std::vector<TagValue>{0x110055llu, 0xff00ffllu}) );


    ON_CALL(database, backend)
        .WillByDefault(Return(&backend));

    ON_CALL(database, utils)
        .WillByDefault(Return(&utils));

    Observer observer;

    // called 6 times by TagInfoCollector for each of TagName after database is set
    //      + 2 times after photo modification (for each tag name)
    EXPECT_CALL(observer, event(_))
        .Times(8);

    TagInfoCollector tagInfoCollector;
    QObject::connect(&tagInfoCollector, &TagInfoCollector::setOfValuesChanged,
                     &observer, &Observer::event);

    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<NiceMock<MockPhotoInfo>>();
    Tag::TagsList tags = {
                            { TagNameInfo(BaseTagsList::Time), TagValue(QTime(20,21)) },
                            { TagNameInfo(BaseTagsList::Event), TagValue(QString("event123")) }
    };

    EXPECT_CALL(*photoInfo, getTags())
        .WillOnce(Return(tags));

    ON_CALL(*photoInfo, getID)
        .WillByDefault(Return(Photo::Id(1)));

    ON_CALL(utils, getPhotoFor(Photo::Id(1)))
        .WillByDefault(Return(photoInfo));

    emit backend.photoModified(photoInfo->getID());
}


TEST(TagInfoCollectorTest, ReactionOnPhotoChange)
{
    NiceMock<MockUtils> utils;
    MockBackend backend;
    NiceMock<MockDatabase> database;

    ON_CALL(database, backend)
        .WillByDefault(Return(&backend));

    ON_CALL(database, utils)
        .WillByDefault(Return(&utils));

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<NiceMock<MockPhotoInfo>>();
    Tag::TagsList tags = {
                            { TagNameInfo(BaseTagsList::Time), TagValue(QTime(2, 5)) },
                            { TagNameInfo(BaseTagsList::Event), TagValue(QString("event123")) }
    };

    EXPECT_CALL(*photoInfo.get(), getTags())
        .WillOnce(Return(tags));

    ON_CALL(*photoInfo, getID)
        .WillByDefault(Return(Photo::Id(1)));

    ON_CALL(utils, getPhotoFor(Photo::Id(1)))
        .WillByDefault(Return(photoInfo));

    emit backend.photoModified(photoInfo->getID());

    auto event = tagInfoCollector.get(TagNameInfo(BaseTagsList::Event));

    ASSERT_EQ(event.size(), 1);
    ASSERT_EQ(event[0].type(), Tag::Type::String);
    EXPECT_EQ(event[0].getString(), "event123");
}
