
#ifndef ANALYZER_PHOTO_CRAWLER_BUILDER_HPP
#define ANALYZER_PHOTO_CRAWLER_BUILDER_HPP

#include <memory>

#include "ianalyzer.hpp"

#include "photos_crawler_export.h"


struct PHOTOS_CRAWLER_EXPORT PhotoCrawlerBuilder final
{
    PhotoCrawlerBuilder();
    ~PhotoCrawlerBuilder();

    std::unique_ptr<IAnalyzer> buildFullFileAnalyzer();
};

#endif
