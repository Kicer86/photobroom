
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QString>

#include "ifile_system_scanner.hpp"
#include "photo_crawler.hpp"
#include "ianalyzer.hpp"


struct FileSystemMock: public IFileSystemScanner
{
    MOCK_METHOD2(getFilesFor, void(const QString &, IFileNotifier *));
    MOCK_METHOD0(stop, void());
};


struct AnalyzerMock: public IAnalyzer
{
    MOCK_METHOD1(isMediaFile, bool(const QString &));
};


TEST(PhotoCrawlerShould, beConstructable)
{
    auto fileSystem = std::make_unique<FileSystemMock>();
    auto analyzer = std::make_unique<AnalyzerMock>();

    using ::testing::_;

    EXPECT_CALL(*fileSystem, getFilesFor(_, _)).Times(0);
    EXPECT_CALL(*fileSystem, stop()).Times(1);
    EXPECT_CALL(*analyzer, isMediaFile(_)).Times(0);
    EXPECT_CALL(*analyzer, isMediaFile(_)).Times(0);

    PhotoCrawler photo_crawler(std::move(fileSystem), std::move(analyzer));
}

/*
TEST(PhotoCrawlerShould, returnMediaFilesForPath)
{
    auto fileSystem = std::make_shared<FileSystemMock>();
    auto analyzer = std::make_shared<AnalyzerMock>();

    using ::testing::Return;

    std::vector<std::string> results = {"a.jpg", "b.jpeg", "c.txt"};

    EXPECT_CALL(*fileSystem, getFilesFor("/path/")).Times(1).WillOnce(Return(results));
    EXPECT_CALL(*analyzer, isImage("c.txt")).Times(1).WillOnce(Return(false));
    EXPECT_CALL(*analyzer, isImage("b.jpeg")).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*analyzer, isImage("a.jpg")).Times(1).WillOnce(Return(true));

    PhotoCrawler photo_crawler(fileSystem, analyzer);

    photo_crawler.crawl("/path/");
}
*/
