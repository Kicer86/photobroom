
#include <gtest/gtest.h>

#include <core/base_tags.hpp>

#include <QDate>
#include <QTime>

#include "database_tools/tag_info_collector.hpp"
#include "unit_tests_utils/mock_database.hpp"

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
        const std::deque<TagValue>& values = tagInfoCollector.get(tag);

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
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::deque<TagValue>({QDate(0, 1, 2), QDate(1, 2, 3)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::deque<TagValue>({QString("event1"), QString("event2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::deque<TagValue>({QTime(2, 3), QTime(3, 4), QTime(11, 18)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::deque<TagValue>({QString("person1"), QString("person2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::deque<TagValue>({QString("12"), QString("23")})) );

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    const std::deque<TagValue>& dates = tagInfoCollector.get( TagNameInfo(BaseTagsList::Date) );
    ASSERT_EQ(dates.size(), 2);
    EXPECT_EQ(dates[0].getDate(), QDate(0, 1, 2));
    EXPECT_EQ(dates[1].getDate(), QDate(1, 2, 3));

    const std::deque<TagValue>& events = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].getString(), "event1");
    EXPECT_EQ(events[1].getString(), "event2");

    const std::deque<TagValue>& times = tagInfoCollector.get( TagNameInfo(BaseTagsList::Time) );
    ASSERT_EQ(times.size(), 3);
    EXPECT_EQ(times[0].getTime(), QTime(2, 3));
    EXPECT_EQ(times[1].getTime(), QTime(3, 4));
    EXPECT_EQ(times[2].getTime(), QTime(11, 18));

    const std::deque<TagValue>& people = tagInfoCollector.get( TagNameInfo(BaseTagsList::People) );
    ASSERT_EQ(people.size(), 2);
    EXPECT_EQ(people[0].getString(), "person1");
    EXPECT_EQ(people[1].getString(), "person2");

    const std::deque<TagValue>& places = tagInfoCollector.get( TagNameInfo(BaseTagsList::Place) );
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
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Date), std::deque<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Event), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::deque<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::deque<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::deque<TagValue>()) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::deque<TagValue>()) );

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    const std::deque<TagValue>& dates = tagInfoCollector.get( TagNameInfo(BaseTagsList::Date) );
    EXPECT_TRUE(dates.empty());

    const std::deque<TagValue>& events = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );
    EXPECT_TRUE(events.empty());

    const std::deque<TagValue>& times = tagInfoCollector.get( TagNameInfo(BaseTagsList::Time) );
    EXPECT_TRUE(times.empty());

    const std::deque<TagValue>& people = tagInfoCollector.get( TagNameInfo(BaseTagsList::People) );
    EXPECT_TRUE(people.empty());

    const std::deque<TagValue>& places = tagInfoCollector.get( TagNameInfo(BaseTagsList::Place) );
    EXPECT_TRUE(places.empty());
}
