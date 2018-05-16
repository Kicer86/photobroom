
#include <gtest/gtest.h>

#include <core/base_tags.hpp>

#include <QDate>
#include <QTime>

#include "database_tools/tag_info_collector.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/mock_photo_info.hpp"


struct Observer
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
    using ::testing::InvokeArgument;
    using ::testing::Return;
    using ::testing::_;

    Database::ADatabaseSignals db_signals;
    MockDatabase database;

    EXPECT_CALL(database, notifier())
        .WillOnce(Return(&db_signals));

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>({QDate(0, 1, 2), QDate(1, 2, 3)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>({QString("event1"), QString("event2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>({QTime(2, 3), QTime(3, 4), QTime(11, 18)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::vector<TagValue>({QString("person1"), QString("person2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>({QString("12"), QString("23")})) );

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

    const std::vector<TagValue>& people = tagInfoCollector.get( TagNameInfo(BaseTagsList::People) );
    ASSERT_EQ(people.size(), 2);
    EXPECT_EQ(people[0].getString(), "person1");
    EXPECT_EQ(people[1].getString(), "person2");

    const std::vector<TagValue>& places = tagInfoCollector.get( TagNameInfo(BaseTagsList::Place) );
    ASSERT_EQ(places.size(), 2);
    EXPECT_EQ(places[0].getString(), "12");
    EXPECT_EQ(places[1].getString(), "23");
}


TEST(TagInfoCollectorTest, EmptyDatabase)
{
    using ::testing::InvokeArgument;
    using ::testing::Return;
    using ::testing::_;

    Database::ADatabaseSignals db_signals;
    MockDatabase database;

    EXPECT_CALL(database, notifier())
        .WillOnce(Return(&db_signals));

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>()) );

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    const std::vector<TagValue>& dates = tagInfoCollector.get( TagNameInfo(BaseTagsList::Date) );
    EXPECT_TRUE(dates.empty());

    const std::vector<TagValue>& events = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );
    EXPECT_TRUE(events.empty());

    const std::vector<TagValue>& times = tagInfoCollector.get( TagNameInfo(BaseTagsList::Time) );
    EXPECT_TRUE(times.empty());

    const std::vector<TagValue>& people = tagInfoCollector.get( TagNameInfo(BaseTagsList::People) );
    EXPECT_TRUE(people.empty());

    const std::vector<TagValue>& places = tagInfoCollector.get( TagNameInfo(BaseTagsList::Place) );
    EXPECT_TRUE(places.empty());
}


TEST(TagInfoCollectorTest, ReactionOnDBChange)
{
    using ::testing::InvokeArgument;
    using ::testing::Return;
    using ::testing::_;

    Database::ADatabaseSignals db_signals;
    MockDatabase database;

    EXPECT_CALL(database, notifier())
        .WillOnce(Return(&db_signals));

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::vector<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>()) );

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<MockPhotoInfo>();
    Tag::TagsList tags = {
                            { TagNameInfo(BaseTagsList::People), TagValue("person123") }
    };

    EXPECT_CALL(*photoInfo.get(), getTags())
        .WillOnce(Return(tags));

    emit db_signals.photoModified(photoInfo);

    const std::vector<TagValue>& people = tagInfoCollector.get( TagNameInfo(BaseTagsList::People) );
    ASSERT_EQ(people.size(), 1);
    EXPECT_EQ(people[0].getString(), "person123");
}


TEST(TagInfoCollectorTest, ObserversNotification)
{
    using ::testing::InvokeArgument;
    using ::testing::Return;
    using ::testing::_;

    Database::ADatabaseSignals db_signals;
    MockDatabase database;

    EXPECT_CALL(database, notifier())
        .WillOnce(Return(&db_signals));

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Date), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::vector<TagValue>({QDate(0, 1, 2), QDate(1, 2, 3)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::vector<TagValue>({QString("event1"), QString("event2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::vector<TagValue>({QTime(2, 3), QTime(3, 4), QTime(11, 18)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::vector<TagValue>({QString("person1"), QString("person2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::vector<TagValue>({QString("12"), QString("23")})) );

    Observer observer;
    using namespace std::placeholders;
    auto observerCallback = std::bind(&Observer::event, &observer, _1);

    // called 5 times by TagInfoCollector for each of TagName after database is set
    //      + 2 times after photo modification (for each tag name)
    EXPECT_CALL(observer, event(_))
        .Times(7);

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.registerChangeObserver(observerCallback);
    tagInfoCollector.set(&database);

    auto photoInfo = std::make_shared<MockPhotoInfo>();
    Tag::TagsList tags = {
                            { TagNameInfo(BaseTagsList::People), TagValue("person123") },
                            { TagNameInfo(BaseTagsList::Event), TagValue("event123") }
    };

    EXPECT_CALL(*photoInfo.get(), getTags())
        .WillOnce(Return(tags));

    emit db_signals.photoModified(photoInfo);
}


TEST(TagInfoCollectorTest, ReactionOnPhotoChange)
{

}
