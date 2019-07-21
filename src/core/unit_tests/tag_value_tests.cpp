
#include <QDate>

#include <gmock/gmock.h>

#include "tag.hpp"

TEST(TagValueTest, EmptyAfterConstruciton)
{
    const TagValue tv;

    EXPECT_EQ(tv.type(), TagValue::Type::Empty);
    EXPECT_TRUE(tv.get().isNull());
    EXPECT_EQ(tv.rawValue(), QString());
}


TEST(TagValueTest, DateSetter)
{
    const QDate date = QDate::currentDate();
    TagValue tv(date);
    const TagValue ctv(date);

    ASSERT_EQ(tv.type(), TagValue::Type::Date);
    ASSERT_EQ(tv.get().type(), QMetaType::QDate);
    EXPECT_EQ(tv.getDate(), date);
    EXPECT_EQ(ctv.getDate(), date);
    EXPECT_EQ(tv.rawValue(), date.toString("yyyy.MM.dd"));
}


TEST(TagValueTest, TimeSetter)
{
    const QTime time = QTime::currentTime();
    TagValue tv(time);
    const TagValue ctv(time);

    ASSERT_EQ(tv.type(), TagValue::Type::Time);
    ASSERT_EQ(tv.get().type(), QMetaType::QTime);
    EXPECT_EQ(tv.getTime(), time);
    EXPECT_EQ(ctv.getTime(), time);
    EXPECT_EQ(tv.rawValue(), time.toString("HH:mm:ss"));
}


TEST(TagValueTest, StringSetter)
{
    const QString str("string test");
    TagValue tv(str);
    const TagValue ctv(str);

    ASSERT_EQ(tv.type(), TagValue::Type::String);
    ASSERT_EQ(tv.get().type(), QMetaType::QString);
    EXPECT_EQ(tv.getString(), str);
    EXPECT_EQ(ctv.getString(), str);
    EXPECT_EQ(tv.rawValue(), str);
}


typedef std::tuple<QString, TagNameInfo::Type, TagValue::Type, QMetaType::Type> TagValueTestExpectations;
struct TagValueTest2: testing::TestWithParam<TagValueTestExpectations> {};

TEST_P(TagValueTest2, rawSetter)
{
    const auto parameters = GetParam();

    const QString raw_value(std::get<0>(parameters));
    TagValue tv = TagValue::fromRaw(raw_value, std::get<1>(parameters));

    ASSERT_EQ(tv.type(), std::get<2>(parameters));
    ASSERT_EQ(tv.get().type(), std::get<3>(parameters));
    EXPECT_EQ(tv.rawValue(), raw_value);
}

INSTANTIATE_TEST_CASE_P(ExtensionsTest,
                        TagValueTest2,
                        testing::Values(
                            TagValueTestExpectations{QString("12:34:56"),
                                                     TagNameInfo::Type::Time,
                                                     TagValue::Type::Time,
                                                     QMetaType::Type::QTime
                            },
                            TagValueTestExpectations{QString("2134.11.03"),
                                                     TagNameInfo::Type::Date,
                                                     TagValue::Type::Date,
                                                     QMetaType::Type::QDate
                            },
                            TagValueTestExpectations{QString("string test"),
                                                     TagNameInfo::Type::String,
                                                     TagValue::Type::String,
                                                     QMetaType::Type::QString
                            }
                        ),
);

/*
typedef std::typle<QVariant, ???> TagValueTestExpectations;
struct TagValueTest2: testing::TestWithParam<TagValueTestExpectations> {};

TEST_P(TagValueTest2, ProperValues)
{
    const auto parameters = GetParam();
    const QVariant valueVar = std::get<0>(parameters);
    const auto variant_extractor = std::get<1>(parameters);
    const auto value =

    TagValue tv(str);
    const TagValue ctv(str);

    ASSERT_EQ(tv.type(), TagValue::Type::String);
    ASSERT_EQ(tv.get().type(), QMetaType::QString);
    EXPECT_EQ(tv.getString(), str);
    EXPECT_EQ(ctv.getString(), str);
    EXPECT_EQ(tv.rawValue(), str);
}

INSTANTIATE_TEST_CASE_P(ExtensionsTest,
                        TagValueTest2,
                        testing::Values(
                            TagValueTestExpectations{QVariant(QDate::currentDate()),
                                                     &QVariant::toDate
                            }
                        ),
);
*/
