
#include "photo_crawler_builder.hpp"

#include "photo_crawler.hpp"
#include "default_filesystem_scanners/filesystemscanner.hpp"
#include "default_analyzers/file_analyzer.hpp"
#include "default_analyzers/file_extension_analyzer.hpp"

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
    analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>("jpe?g") );
    analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>("png") );
    analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>("gif") );
    analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>("tiff?") );
    analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>("bmp") );

    // TODO: added due to bug in clang: http://stackoverflow.com/questions/36752678/clang-returning-stdunique-ptr-with-type-conversion
    return std::move(analyzer);
}
