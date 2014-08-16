
#include "photo_crawler_builder.hpp"

#include "photo_crawler.hpp"
#include "file_analyzer.hpp"
#include "filesystemscanner.hpp"
#include "default_analyzers/ext_defaultanalyzer.hpp"

PhotoCrawlerBuilder::PhotoCrawlerBuilder()
{
}

PhotoCrawlerBuilder::~PhotoCrawlerBuilder()
{
}

IPhotoCrawler* PhotoCrawlerBuilder::build()
{
    auto analyzer = std::make_shared<FileAnalyzer>();

    //add subanalyzers
    analyzer->registerAnalyzer(new Ext_DefaultAnalyzer);

    //file system scanner
    auto scanner = std::make_shared<FileSystemScanner>();

    //build crawler
    IPhotoCrawler* crawler = new PhotoCrawler(scanner, analyzer);

    return crawler;
}
