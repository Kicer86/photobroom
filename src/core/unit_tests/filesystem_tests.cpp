
#include <string_view>

#include <QFile>
#include <QTemporaryDir>
#include <gtest/gtest.h>

#include "filesystem.hpp"


TEST(FilesystemLocationTest, KeepsQStringView)
{
    const QString path = QStringLiteral("photos/image.jpg");
    const Filesystem::Location location{QStringView(path)};

    EXPECT_EQ(location.toQStr(), path);
    EXPECT_EQ(QString::fromStdString(location.toStr()), path);
}


TEST(FilesystemLocationTest, KeepsStdStringView)
{
    constexpr std::string_view path = "photos/image.jpg";
    const Filesystem::Location location{path};

    EXPECT_EQ(location.toStr(), path);
    EXPECT_EQ(location.toQStr(), QString::fromUtf8(path.data(), path.size()));
}


TEST(FilesystemOpenFileTest, ReturnsNullForMissingFile)
{
    QTemporaryDir temporaryDirectory;
    ASSERT_TRUE(temporaryDirectory.isValid());

    const auto path = temporaryDirectory.filePath(QStringLiteral("missing.bin"));
    const auto file = Filesystem::openFile(Filesystem::Location{QStringView(path)});

    EXPECT_EQ(file, nullptr);
}


TEST(FilesystemOpenFileTest, ReadsExistingFileThroughBothViews)
{
    QTemporaryDir temporaryDirectory;
    ASSERT_TRUE(temporaryDirectory.isValid());

    const auto path = temporaryDirectory.filePath(QStringLiteral("data.bin"));
    const QByteArray expected("file contents\0with a zero", 25);

    QFile output(path);
    ASSERT_TRUE(output.open(QIODevice::WriteOnly));
    ASSERT_EQ(output.write(expected), expected.size());
    output.close();

    const auto file = Filesystem::openFile(Filesystem::Location{QStringView(path)});
    ASSERT_NE(file, nullptr);

    const auto qArrayView = file->asQArrayView();
    const auto byteView = file->byteView();

    EXPECT_EQ(qArrayView, expected);
    ASSERT_EQ(byteView.size(), static_cast<std::size_t>(expected.size()));
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(byteView.data()), byteView.size()),
              std::string_view(expected.constData(), static_cast<std::size_t>(expected.size())));
}


TEST(FilesystemOpenFileTest, OpensEmptyFile)
{
    QTemporaryDir temporaryDirectory;
    ASSERT_TRUE(temporaryDirectory.isValid());

    const auto path = temporaryDirectory.filePath(QStringLiteral("empty.bin"));
    QFile output(path);
    ASSERT_TRUE(output.open(QIODevice::WriteOnly));
    output.close();

    const auto file = Filesystem::openFile(Filesystem::Location{QStringView(path)});
    ASSERT_NE(file, nullptr);

    EXPECT_TRUE(file->asQArrayView().isEmpty());
    EXPECT_TRUE(file->byteView().empty());
}
