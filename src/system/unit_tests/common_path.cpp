
#include <gmock/gmock.h>

#include <QString>

#include "unit_tests_utils/printers.hpp"

import broom.system;


TEST(FileSystemTest, EqualFilePaths)
{
    const FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/file.ext", "/foo/bar/file.ext");

    EXPECT_EQ(common, "/foo/bar/file.ext");
}


TEST(FileSystemTest, EqualDirPaths)
{
    const FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/", "/foo/bar/");

    EXPECT_EQ(common, "/foo/bar/");
}


TEST(FileSystemTest, EqualDirPaths2)
{
    const FileSystem fs;

    const QString common = fs.commonPath("/foo/bar", "/foo/bar");

    EXPECT_EQ(common, "/foo/bar");
}


TEST(FileSystemTest, DifferentFileNamesWithCommonPart)
{
    const FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/file1.png", "/foo/bar/file2.png");

    EXPECT_EQ(common, "/foo/bar/");
}


TEST(FileSystemTest, DifferentFileNamesWithoutCommonPart)
{
    const FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/1.png", "/foo/bar/2.png");

    EXPECT_EQ(common, "/foo/bar/");
}


TEST(FileSystemTest, SameRelativeFilePath)
{
    const FileSystem fs;

    const QString common = fs.commonPath("../bar/file.png", "../bar/file.png");

    EXPECT_EQ(common, "../bar/file.png");
}


TEST(FileSystemTest, SameRelativeFilePath2)
{
    const FileSystem fs;

    const QString common = fs.commonPath("file.png", "file.png");

    EXPECT_EQ(common, "file.png");
}


TEST(FileSystemTest, TotallyDifferentPaths)
{
    const FileSystem fs;

    const QString common = fs.commonPath("/bar/foo/file.png", "/foo/bar/file.png");

    EXPECT_EQ(common, "/");
}


TEST(FileSystemTest, TotallyDifferentRelativePaths)
{
    const FileSystem fs;

    const QString common = fs.commonPath("bar/foo/file.png", "foo/bar/file.png");

    EXPECT_EQ(common, "");
}


TEST(FileSystemTest, DifferentPathsWhichCommonPrefix)
{
    const FileSystem fs;

    const QString common = fs.commonPath("xx:/bar/foo/file.png", "xx:/foo/bar/file.png");

    EXPECT_EQ(common, "xx:/");
}
