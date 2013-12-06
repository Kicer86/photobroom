
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "implementation/ifile_system_scanner.hpp"
#include "implementation/photo_crawler.hpp"
#include "ianalyzer.hpp"


struct FileSystemMock: public IFileSystemScanner
{
    MOCK_METHOD1(getFilesFor, std::vector<std::string>(const std::string &));
};


struct AnalyzerMock: public IAnalyzer
{
    MOCK_METHOD1(isImage, bool(const std::string &));
};


TEST(PhotoCrawlerShould, beConstructable)
{
    auto fileSystem = std::make_shared<FileSystemMock>();
    auto analyzer = std::make_shared<AnalyzerMock>();
    
    using ::testing::_;
    
    EXPECT_CALL(*fileSystem, getFilesFor(_)).Times(0);
    EXPECT_CALL(*analyzer, isImage(_)).Times(0);
    
    PhotoCrawler photo_crawler(fileSystem, analyzer);
}


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


TEST(PhotoCrawlerShould, allowToSetRules)
{
    auto fileSystem = std::make_shared<FileSystemMock>();
    auto analyzer = std::make_shared<AnalyzerMock>();
    
    using ::testing::_;
    
    EXPECT_CALL(*fileSystem, getFilesFor(_)).Times(0);
    EXPECT_CALL(*analyzer, isImage(_)).Times(0);
    
    PhotoCrawler photo_crawler(fileSystem, analyzer);
        
    Rules rules;
    photo_crawler.setRules(rules);
}
