
#include <gtest/gtest.h>

#include <core/base_tags.hpp>

#include <QDate>
#include <QTime>

#include "database_tools/tag_info_collector.hpp"
#include "unit_tests_utils/mock_database.hpp"

TEST(TagInfoCollectorTest, isConstructible)
{
    EXPECT_NO_THROW(
    {
        TagInfoCollector tagInfoCollector;
    });
}


TEST(TagInfoCollectorTest, getsWithoutDatabase)
{
    TagInfoCollector tagInfoCollector;

    std::vector<BaseTagsList> tags = BaseTags::getAll();

    for(const BaseTagsList& tag: tags)
    {
        const std::set<TagValue>& values = tagInfoCollector.get(tag);

        EXPECT_EQ(values.empty(), true);
    }
}


TEST(TagInfoCollectorTest, returnsValuesFromDatabase)
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
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Event), std::deque<TagValue>({QString("1"), QString("2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Time), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Time), std::deque<TagValue>({QTime(2, 3), QTime(3, 4)})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::People), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::People), std::deque<TagValue>({QString("1"), QString("2")})) );

    EXPECT_CALL(database, listTagValues(TagNameInfo(BaseTagsList::Place), _))
        .WillOnce( InvokeArgument<1>(TagNameInfo(BaseTagsList::Place), std::deque<TagValue>({QString("12"), QString("23")})) );

    TagInfoCollector tagInfoCollector;
    tagInfoCollector.set(&database);

    const std::set<TagValue>& events = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );
    ASSERT_EQ(events.size(), 2);
}
