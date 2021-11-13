
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QTimeZone>

#include "implementation/exiftool_video_details_reader.hpp"
#include "unit_tests_utils/printers.hpp"


using namespace testing;

TEST(ExifToolUtils, parser)
{
    const QStringList output =
    {
        "   qwe    :abc     ",
        "xxc:abc            ",
        "   sdf:        dddd",
        ":d                 ",
        "e:                 "
    };

    const auto entries = ExiftoolUtils::parseOutput(output);

    EXPECT_THAT(entries, UnorderedElementsAre
    (
        std::pair<QString, QString>("qwe", "abc"),
        std::pair<QString, QString>("xxc", "abc"),
        std::pair<QString, QString>("sdf", "dddd"),
        std::pair<QString, QString>("",    "d"),
        std::pair<QString, QString>("e",   "")
    ));
}


TEST(ExiftoolVideoDetailsReaderTest, resolution)
{
    std::map<QString, QString> entries =
    {
        {"Image Width", "100"},
        {"Image Height", "200"},
        {"Rotation", "90"}
    };
    const ExiftoolVideoDetailsReader reader(entries);
    const std::optional<QSize> res = reader.resolutionOf();

    ASSERT_TRUE(res);
    EXPECT_EQ(*res, QSize(200, 100));
}


TEST(ExiftoolVideoDetailsReaderTest, duration1)
{
    std::map<QString, QString> entries =
    {
        {"Duration", "12.56 s"}
    };
    const ExiftoolVideoDetailsReader reader(entries);
    const std::optional<int> dur = reader.durationOf();

    ASSERT_TRUE(dur);
    EXPECT_EQ(*dur, 12);
}


TEST(ExiftoolVideoDetailsReaderTest, duration2)
{
    std::map<QString, QString> entries =
    {
        {"Duration", "0:01:28"}
    };
    const ExiftoolVideoDetailsReader reader(entries);
    const std::optional<int> dur = reader.durationOf();

    ASSERT_TRUE(dur);
    EXPECT_EQ(*dur, 88);
}


TEST(ExiftoolVideoDetailsReaderTest, creationTime1)
{
    std::map<QString, QString> entries =
    {
        {"Date/Time Original", "2021:10:06 13:07:17+02:00 DST"}
    };
    const ExiftoolVideoDetailsReader reader(entries);
    const std::optional<QDateTime> creation = reader.creationTime();

    ASSERT_TRUE(creation);
    EXPECT_EQ(*creation, QDateTime(QDate(2021, 10, 6), QTime(13, 7, 17)));
}


TEST(ExiftoolVideoDetailsReaderTest, creationTime2)
{
    std::map<QString, QString> entries =
    {
        {"Create Date", "2021:08:21 09:46:18"}
    };
    const ExiftoolVideoDetailsReader reader(entries);
    const std::optional<QDateTime> creation = reader.creationTime();

    ASSERT_TRUE(creation);

    const QDateTime expectedDateUtc(QDate(2021, 8, 21), QTime(9, 46, 18), QTimeZone::utc());
    const QDateTime expectedDate = expectedDateUtc.toLocalTime();
    EXPECT_EQ(*creation, expectedDate);
}
