
#ifndef ANALYZER_PHOTO_CRAWLER_BUILDER_HPP
#define ANALYZER_PHOTO_CRAWLER_BUILDER_HPP

#include <memory>

#include "photos_crawler_export.h"

struct IPhotoCrawler;

struct PHOTOS_CRAWLER_EXPORT PhotoCrawlerBuilder
{
    PhotoCrawlerBuilder();
    virtual ~PhotoCrawlerBuilder();

    IPhotoCrawler* build();
};

#endif
