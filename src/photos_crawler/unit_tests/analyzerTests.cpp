
#include "default_analyzers/file_analyzer.hpp"

#include <gtest/gtest.h>

#include <QString>

TEST(Ext_DefaultAnalyzerShould, ReturnTrueForJpegFiles)
{
    FileAnalyzer analyzer;

    const bool status1 = analyzer.isMediaFile("/home/image.jpg");
    const bool status2 = analyzer.isMediaFile("/home/image.jpeg");
    const bool status3 = analyzer.isMediaFile("/home/image.jp");
    const bool status4 = analyzer.isMediaFile("/home/image.png");
    const bool status5 = analyzer.isMediaFile("image.jpg");
    const bool status6 = analyzer.isMediaFile("image.JpeG");
    const bool status7 = analyzer.isMediaFile("image.jpg3");
    const bool status8 = analyzer.isMediaFile("/home/image .jpg");

    ASSERT_EQ(true, status1);
    ASSERT_EQ(true, status2);
    ASSERT_EQ(false, status3);
    ASSERT_EQ(true, status4);
    ASSERT_EQ(true, status5);
    ASSERT_EQ(true, status6);
    ASSERT_EQ(false, status7);
    ASSERT_EQ(true, status8);
}
