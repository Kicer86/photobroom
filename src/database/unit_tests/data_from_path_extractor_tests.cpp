
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "database_tools/implementation/data_from_path_extractor.cpp"


using testing::ElementsAre;
using testing::UnorderedElementsAre;


TEST(DataFromPathExtractorTest, NoDate)
{
    DataFromPathExtractor extractor;

    const auto tags1 = extractor.extract("qwer/rertty/11/23/fdgfg/h.jpeg");
    const auto tags2 = extractor.extract("");
    const auto tags3 = extractor.extract({});
    const auto tags4 = extractor.extract("00000000000000000000000000000000000000000000000000000000");

    EXPECT_TRUE(tags1.empty());
    EXPECT_TRUE(tags2.empty());
    EXPECT_TRUE(tags3.empty());
    EXPECT_TRUE(tags4.empty());
}


TEST(DataFromPathExtractorTest, InvalidDate)
{
    DataFromPathExtractor extractor;

    const auto tags1 = extractor.extract("2010-13-12");
    const auto tags2 = extractor.extract("2010-01-32");
    const auto tags3 = extractor.extract("scrapbooking/1301092178_181016502_1-fdg--fdh-jhgyt-jhgjy-htfh-u-ghdtjhy.jpg");
    const auto tags4 = extractor.extract("gh ji/hfh/hfgj/Rgfhgh/120408105348.jpg");
    const auto tags5 = extractor.extract("d fg/vdgf/received_892712301075716.jpeg");

    EXPECT_TRUE(tags1.empty());
    EXPECT_TRUE(tags2.empty());
    EXPECT_TRUE(tags3.empty());
    EXPECT_TRUE(tags4.empty());
    EXPECT_TRUE(tags5.empty());
}


TEST(DataFromPathExtractorTest, ValidDate)
{
    DataFromPathExtractor extractor;

    const auto tags1 = extractor.extract("Mandrake.20071106.gif");
    const auto tags2 = extractor.extract("2022-12-50_11-99-99/2010.10-11/11112010039.mp4");
    const auto tags3 = extractor.extract("/2010.05.22-23/DSCF8415.mkv");
    const auto tags4 = extractor.extract("2017-06-01-QQQ-WWW/024.jpg");

    EXPECT_THAT( tags1, ElementsAre( std::pair{TagTypes::Date, QDate(2007, 11, 06)} ) );
    EXPECT_THAT( tags2, ElementsAre( std::pair{TagTypes::Date, QDate(2010, 10, 11)} ) );
    EXPECT_THAT( tags3, ElementsAre( std::pair{TagTypes::Date, QDate(2010, 05, 22)} ) );
    EXPECT_THAT( tags4, ElementsAre( std::pair{TagTypes::Date, QDate(2017, 06, 01)} ) );
}


TEST(DataFromPathExtractorTest, ValidDateAndTime)
{
    DataFromPathExtractor extractor;

    const auto tags1 = extractor.extract("WP_20150705_135843Z.mp4");
    const auto tags2 = extractor.extract("/dir/Camera 2/20141122_164931.mp4");
    const auto tags3 = extractor.extract("qq/ww/2008-07-06_13-55-04_1280x1024_screenshot.png");
    const auto tags4 = extractor.extract("/2010-12-07-21_55_11.png");
    const auto tags5 = extractor.extract("jk/2020-12-01_19_05_39 1-0.png");
    const auto tags6 = extractor.extract("mm/20161215_092517.mp4");
    const auto tags7 = extractor.extract("20005678-123456/WP_20180123_09_44_08_Pro.mp4");
    const auto tags8 = extractor.extract("/2015-10-20/20151020183114.MTS");
    const auto tags9 = extractor.extract("2020-07-21-sdfgg_t_kjyy/2020-07-21/20200721104521.MTS");
    const auto tags10 = extractor.extract("q erty/vcxjfdsfjk/gfkrtklbvj/2010-01-16 23;57;20.jpg");

    EXPECT_THAT( tags1, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2015, 07, 05)}, std::pair{TagTypes::Time, QTime(13, 58, 43)} ) );
    EXPECT_THAT( tags2, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2014, 11, 22)}, std::pair{TagTypes::Time, QTime(16, 49, 31)} ) );
    EXPECT_THAT( tags3, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2008, 07, 06)}, std::pair{TagTypes::Time, QTime(13, 55, 04)} ) );
    EXPECT_THAT( tags4, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2010, 12, 07)}, std::pair{TagTypes::Time, QTime(21, 55, 11)} ) );
    EXPECT_THAT( tags5, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2020, 12, 01)}, std::pair{TagTypes::Time, QTime(19, 05, 39)} ) );
    EXPECT_THAT( tags6, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2016, 12, 15)}, std::pair{TagTypes::Time, QTime( 9, 25, 17)} ) );
    EXPECT_THAT( tags7, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2018, 01, 23)}, std::pair{TagTypes::Time, QTime( 9, 44,  8)} ) );
    EXPECT_THAT( tags8, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2015, 10, 20)}, std::pair{TagTypes::Time, QTime(18, 31, 14)} ) );
    EXPECT_THAT( tags9, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2020, 07, 21)}, std::pair{TagTypes::Time, QTime(10, 45, 21)} ) );
    EXPECT_THAT( tags10, UnorderedElementsAre( std::pair{TagTypes::Date, QDate(2010, 01, 16)}, std::pair{TagTypes::Time, QTime(23, 57, 20)} ) );
}
