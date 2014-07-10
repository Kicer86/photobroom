
#ifndef ANALYZER_PHOTO_CRAWLER_BUILDER_HPP
#define ANALYZER_PHOTO_CRAWLER_BUILDER_HPP

#include <memory>

#include "analyzer_export.h"

struct IPhotoCrawler;

struct ANALYZER_EXPORT PhotoCrawlerBuilder
{
    PhotoCrawlerBuilder();
    virtual ~PhotoCrawlerBuilder();

    IPhotoCrawler* build();
};

#endif
