
#include "photo_crawler_builder.hpp"

#include "photo_crawler.hpp"
#include "default_filesystem_scanners/filesystemscanner.hpp"
#include "default_analyzers/ext_defaultanalyzer.hpp"
#include "default_analyzers/file_analyzer.hpp"

PhotoCrawlerBuilder::PhotoCrawlerBuilder()
{
}

PhotoCrawlerBuilder::~PhotoCrawlerBuilder()
{
}

std::unique_ptr<IAnalyzer> PhotoCrawlerBuilder::buildFullFileAnalyzer()
{
    auto analyzer = std::make_unique<FileAnalyzer>();

    //add subanalyzers
    analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>() );

    // TODO: added due to bug in clang: http://stackoverflow.com/questions/36752678/clang-returning-stdunique-ptr-with-type-conversion
    return std::move(analyzer);
}
