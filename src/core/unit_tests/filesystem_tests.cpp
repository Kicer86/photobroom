
#include <memory>
#include <string_view>

#include <gtest/gtest.h>

#include "filesystem.hpp"
#include "unit_tests_utils/temporary_files.hpp"


namespace
{
    std::unique_ptr<Filesystem::IFile> openTestFile(const Filesystem::Location& location)
    {
        return Filesystem::openFile(location);
    }
}


TEST(FilesystemLocationTest, KeepsQStringView)
{
    const QString path = QStringLiteral("photos/image.jpg");
    const Filesystem::Location location{QStringView(path)};

    EXPECT_EQ(location.url(), path);
}


TEST(FilesystemLocationTest, KeepsStdStringView)
{
    constexpr std::string_view path = "photos/image.jpg";
    const Filesystem::Location location{path};

    EXPECT_EQ(location.url(), path);
    EXPECT_EQ(location.url(), QString::fromUtf8(path.data(), path.size()));
}


TEST(FilesystemOpenFileTest, ReturnsNotOpenForMissingFile)
{
    const UnitTests::TemporaryFiles temporaryFiles;
    const auto file = openTestFile(temporaryFiles.location(u"missing.bin"));

    EXPECT_EQ(file->openMode(), QIODeviceBase::NotOpen);
}


TEST(FilesystemOpenFileTest, ReadsExistingFileThroughBothViews)
{
    const UnitTests::TemporaryFiles temporaryFiles;
    const QByteArray expected("file contents\0with a zero", 25);

    const auto file = openTestFile(temporaryFiles.createFile(u"data.bin", expected));
    ASSERT_NE(file, nullptr);

    const auto qArrayView = file->asQArrayView();
    const auto byteView = file->byteView();

    EXPECT_EQ(qArrayView, expected);
    ASSERT_EQ(byteView.size(), static_cast<std::size_t>(expected.size()));
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(byteView.data()), byteView.size()),
              std::string_view(expected.constData(), static_cast<std::size_t>(expected.size())));
}


TEST(FilesystemOpenFileTest, ExposesSizeAndReadStateThroughQIODevice)
{
    const UnitTests::TemporaryFiles temporaryFiles;
    const QByteArray expected("abcdef");

    const auto file = openTestFile(temporaryFiles.createFile(u"data.bin", expected));
    ASSERT_NE(file, nullptr);
    ASSERT_TRUE(file->isOpen());
    EXPECT_TRUE(file->isReadable());
    EXPECT_FALSE(file->isWritable());
    EXPECT_FALSE(file->isSequential());

    EXPECT_EQ(file->size(), expected.size());
    EXPECT_EQ(file->pos(), 0);
    EXPECT_EQ(file->bytesAvailable(), expected.size());
    EXPECT_FALSE(file->atEnd());

    EXPECT_EQ(file->read(2), expected.left(2));
    EXPECT_EQ(file->pos(), 2);
    EXPECT_EQ(file->bytesAvailable(), expected.size() - 2);
    EXPECT_FALSE(file->atEnd());

    EXPECT_EQ(file->readAll(), expected.mid(2));
    EXPECT_EQ(file->pos(), expected.size());
    EXPECT_EQ(file->bytesAvailable(), 0);
    EXPECT_TRUE(file->atEnd());
}


TEST(FilesystemOpenFileTest, SeeksUnderlyingDeviceBackwards)
{
    const UnitTests::TemporaryFiles temporaryFiles;
    const QByteArray expected("abcdef");

    const auto file = openTestFile(temporaryFiles.createFile(u"data.bin", expected));
    ASSERT_NE(file, nullptr);
    ASSERT_TRUE(file->isOpen());

    EXPECT_EQ(file->read(4), expected.left(4));
    ASSERT_TRUE(file->seek(1));
    EXPECT_EQ(file->pos(), 1);
    EXPECT_EQ(file->read(2), expected.mid(1, 2));
}


TEST(FilesystemOpenFileTest, CanBeClosedAndReopened)
{
    const UnitTests::TemporaryFiles temporaryFiles;
    const QByteArray expected("abcdef");

    const auto file = openTestFile(temporaryFiles.createFile(u"data.bin", expected));
    ASSERT_NE(file, nullptr);
    ASSERT_TRUE(file->isOpen());
    EXPECT_EQ(file->asQArrayView(), expected);

    file->close();
    EXPECT_FALSE(file->isOpen());

    ASSERT_TRUE(file->open(QIODevice::ReadOnly));
    EXPECT_EQ(file->pos(), 0);
    EXPECT_EQ(file->readAll(), expected);
    EXPECT_EQ(file->asQArrayView(), expected);
}


TEST(FilesystemOpenFileTest, OpensEmptyFile)
{
    const UnitTests::TemporaryFiles temporaryFiles;

    const auto file = openTestFile(temporaryFiles.createFile(u"empty.bin"));
    ASSERT_NE(file, nullptr);

    EXPECT_EQ(file->size(), 0);
    EXPECT_EQ(file->bytesAvailable(), 0);
    EXPECT_TRUE(file->atEnd());
    EXPECT_TRUE(file->readAll().isEmpty());
    EXPECT_TRUE(file->asQArrayView().isEmpty());
    EXPECT_TRUE(file->byteView().empty());
}
