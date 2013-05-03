
#ifndef ANALYZER_PHOTO_CRAWLER_BUILDER_HPP
#define ANALYZER_PHOTO_CRAWLER_BUILDER_HPP

#include <memory>

struct IPhotoCrawler;

struct PhotoCrawlerBuilder
{
	PhotoCrawlerBuilder();
	virtual ~PhotoCrawlerBuilder();

	std::shared_ptr<IPhotoCrawler> build();
};

#endif
