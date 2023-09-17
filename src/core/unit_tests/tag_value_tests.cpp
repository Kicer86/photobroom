
#include <QColor>
#include <QDate>
#include <QModelIndex>

#include <gmock/gmock.h>

#include "tag.hpp"


TEST(TagValueTest, EmptyAfterConstruciton)
{
    const TagValue tv;

    EXPECT_EQ(tv.type(), Tag::ValueType::Empty);
    EXPECT_TRUE(tv.get().isNull());
    EXPECT_EQ(tv.rawValue(), QString());
}


TEST(TagValueTest, DateSetter)
{
    const QDate date = QDate::currentDate();
    TagValue tv(date);
    const TagValue ctv(date);

    ASSERT_EQ(tv.type(), Tag::ValueType::Date);
    ASSERT_EQ(tv.get().typeId(), QMetaType::Type::QDate);
    EXPECT_EQ(tv.getDate(), date);
    EXPECT_EQ(ctv.getDate(), date);
    EXPECT_EQ(tv.rawValue(), date.toString("yyyy.MM.dd"));
}


TEST(TagValueTest, TimeSetter)
{
    const QTime time = QTime::currentTime();
    TagValue tv(time);
    const TagValue ctv(time);

    ASSERT_EQ(tv.type(), Tag::ValueType::Time);
    ASSERT_EQ(tv.get().typeId(), QMetaType::Type::QTime);
    EXPECT_EQ(tv.getTime(), time);
    EXPECT_EQ(ctv.getTime(), time);
    EXPECT_EQ(tv.rawValue(), time.toString("HH:mm:ss"));
}


TEST(TagValueTest, StringSetter)
{
    const QString str("string test");
    TagValue tv(str);
    const TagValue ctv(str);

    ASSERT_EQ(tv.type(), Tag::ValueType::String);
    ASSERT_EQ(tv.get().typeId(), QMetaType::Type::QString);
    EXPECT_EQ(tv.getString(), str);
    EXPECT_EQ(ctv.getString(), str);
    EXPECT_EQ(tv.rawValue(), str);
}


TEST(TagValueTest, IntSetter)
{
    const int value = 123;
    TagValue tv(value);
    const TagValue ctv(value);

    ASSERT_EQ(tv.type(), Tag::ValueType::Int);
    ASSERT_EQ(tv.get().typeId(), QMetaType::Type::Int);
    EXPECT_EQ(tv.get(), value);
    EXPECT_EQ(tv.rawValue(), QString::number(value));
}


TEST(TagValueTest, ColorSetter)
{
    const QColor value = Qt::red;
    TagValue tv(value);
    const TagValue ctv(value);

    ASSERT_EQ(tv.type(), Tag::ValueType::Color);
    ASSERT_EQ(tv.get().typeId(), QMetaType::Type::QColor);
    EXPECT_EQ(tv.get(), value);
    EXPECT_EQ(tv.rawValue(), QString::number(value.rgba64()));
}


TEST(TagValueTest, CopyOperations)
{
    const TagValue tv1 = TagValue::fromQVariant(QDate::currentDate());
    const TagValue tv2(tv1);
    TagValue tv3;
    tv3 = tv2;

    EXPECT_EQ(tv1, tv2);
    EXPECT_EQ(tv2, tv3);
}


TEST(TagValueTest, CompareOperation)
{
    const std::vector<TagValue> tag_values =
    {
        TagValue::fromQVariant(QDate::currentDate()),
        TagValue::fromQVariant(QTime::currentTime()),
        TagValue::fromQVariant(QString("test string"))
    };

    const std::size_t s = tag_values.size();

    for(std::size_t i = 0; i < s; i++)
        for(std::size_t j = 0; j < s; j++)
        {
            const TagValue& l = tag_values[i];
            const TagValue& r = tag_values[j];

            if (i == j)
            {
                EXPECT_TRUE(l == r);
                EXPECT_FALSE(l != r);
                EXPECT_FALSE(l < r);
                EXPECT_FALSE(r < l);
            }
            else
            {
                EXPECT_FALSE(l == r);
                EXPECT_TRUE(l != r);
                EXPECT_NE(l < r, r < l);
            }
        }
}


TEST(TagValueTest, MoveOperations)
{
    TagValue tv1 = TagValue::fromQVariant(QDate::currentDate());
    const TagValue tv2 = tv1;
    const TagValue tv3(std::move(tv1));

    EXPECT_EQ(tv2, tv3);
    EXPECT_EQ(tv1.type(), Tag::ValueType::Empty);
}


typedef std::tuple<QString, Tag::ValueType, Tag::ValueType, QMetaType::Type> TagValueRawSetterTestExpectations;
struct TagValueRawSetterTest: testing::TestWithParam<TagValueRawSetterTestExpectations> {};

TEST_P(TagValueRawSetterTest, rawSetter)
{
    const auto parameters = GetParam();

    const QString raw_value(std::get<0>(parameters));
    TagValue tv = TagValue::fromRaw(raw_value, std::get<1>(parameters));

    ASSERT_EQ(tv.type(), std::get<2>(parameters));
    ASSERT_EQ(tv.get().typeId(), std::get<3>(parameters));
    EXPECT_EQ(tv.rawValue(), raw_value);
}

INSTANTIATE_TEST_SUITE_P(ExtensionsTest,
                         TagValueRawSetterTest,
                         testing::Values(
                             TagValueRawSetterTestExpectations{QString("12:34:56"),
                                                               Tag::ValueType::Time,
                                                               Tag::ValueType::Time,
                                                               QMetaType::Type::QTime
                             },
                             TagValueRawSetterTestExpectations{QString("2134.11.03"),
                                                               Tag::ValueType::Date,
                                                               Tag::ValueType::Date,
                                                               QMetaType::Type::QDate
                             },
                             TagValueRawSetterTestExpectations{QString("string test"),
                                                               Tag::ValueType::String,
                                                               Tag::ValueType::String,
                                                               QMetaType::Type::QString
                             }
                         )
);


typedef std::tuple<QVariant, Tag::ValueType, QMetaType::Type> TagValueVariantSetterTestExpectations;
struct TagValueVariantSetterTest: testing::TestWithParam<TagValueVariantSetterTestExpectations> {};

TEST_P(TagValueVariantSetterTest, variantSetter)
{
    const auto parameters = GetParam();

    const QVariant raw_value(std::get<0>(parameters));
    TagValue tv = TagValue::fromQVariant(raw_value);

    ASSERT_EQ(tv.type(), std::get<1>(parameters));
    ASSERT_EQ(tv.get().typeId(), std::get<2>(parameters));
    EXPECT_EQ(tv.get(), raw_value);
}

INSTANTIATE_TEST_SUITE_P(ExtensionsTest,
                         TagValueVariantSetterTest,
                         testing::Values(
                             TagValueVariantSetterTestExpectations{QVariant(QTime::currentTime()),
                                                                   Tag::ValueType::Time,
                                                                   QMetaType::Type::QTime
                             },
                             TagValueVariantSetterTestExpectations{QVariant(QDate::currentDate()),
                                                                   Tag::ValueType::Date,
                                                                   QMetaType::Type::QDate
                             },
                             TagValueVariantSetterTestExpectations{QVariant(QString("string test")),
                                                                   Tag::ValueType::String,
                                                                   QMetaType::Type::QString
                             }
                         )
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

    ASSERT_EQ(tv.type(), Tag::ValueType::String);
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
