
#include "photo_crawler_builder.hpp"

#include <core/media_types.hpp>

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
    for(const char* ext: MediaTypes::all_extensions)
        analyzer->registerAnalyzer( std::make_unique<FileExtensionAnalyzer>(ext) );

    // TODO: added due to bug in clang: http://stackoverflow.com/questions/36752678/clang-returning-stdunique-ptr-with-type-conversion
    return std::move(analyzer);
}
