
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

std::unique_ptr<IPhotoCrawler> PhotoCrawlerBuilder::build()
{
    auto analyzer = std::make_unique<FileAnalyzer>();

    //add subanalyzers
    analyzer->registerAnalyzer( std::make_unique<Ext_DefaultAnalyzer>() );

    //file system scanner
    auto scanner = std::make_unique<FileSystemScanner>();

    //build crawler
    auto crawler = std::make_unique<PhotoCrawler>(std::move(scanner), std::move(analyzer));

    return crawler;
}
