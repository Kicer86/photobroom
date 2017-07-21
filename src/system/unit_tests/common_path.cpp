
#include <gmock/gmock.h>

#include "../filesystem.hpp"
#include "unit_tests_utils/printers.hpp"

TEST(FileSystemTest, EqualFilePaths)
{
    FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/file.ext", "/foo/bar/file.ext");

    EXPECT_EQ(common, "/foo/bar/file.ext");
}


TEST(FileSystemTest, EqualDirPaths)
{
    FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/", "/foo/bar/");

    EXPECT_EQ(common, "/foo/bar/");
}


TEST(FileSystemTest, EqualDirPaths2)
{
    FileSystem fs;

    const QString common = fs.commonPath("/foo/bar", "/foo/bar");

    EXPECT_EQ(common, "/foo/bar");
}


TEST(FileSystemTest, DifferentFileNamesWithCommonPart)
{
    FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/file1.png", "/foo/bar/file2.png");

    EXPECT_EQ(common, "/foo/bar/");
}


TEST(FileSystemTest, DifferentFileNamesWithoutCommonPart)
{
    FileSystem fs;

    const QString common = fs.commonPath("/foo/bar/1.png", "/foo/bar/2.png");

    EXPECT_EQ(common, "/foo/bar/");
}


TEST(FileSystemTest, SameRelativeFilePath)
{
    FileSystem fs;

    const QString common = fs.commonPath("../bar/file.png", "../bar/file.png");

    EXPECT_EQ(common, "../bar/file.png");
}


TEST(FileSystemTest, SameRelativeFilePath2)
{
    FileSystem fs;

    const QString common = fs.commonPath("file.png", "file.png");

    EXPECT_EQ(common, "file.png");
}


TEST(FileSystemTest, TotallyDifferentPaths)
{
    FileSystem fs;

    const QString common = fs.commonPath("/bar/foo/file.png", "/foo/bar/file.png");

    EXPECT_EQ(common, "/");
}


TEST(FileSystemTest, TotallyDifferentRelativePaths)
{
    FileSystem fs;

    const QString common = fs.commonPath("bar/foo/file.png", "foo/bar/file.png");

    EXPECT_EQ(common, "");
}


TEST(FileSystemTest, DifferentPathsWhichCommonPrefix)
{
    FileSystem fs;

    const QString common = fs.commonPath("xx:/bar/foo/file.png", "xx:/foo/bar/file.png");

    EXPECT_EQ(common, "xx:/");
}
