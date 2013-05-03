
#ifndef ANALYZER_PHOTO_CRAWLER_BUILDER_HPP
#define ANALYZER_PHOTO_CRAWLER_BUILDER_HPP

#include <memory>

#include "analyzer_exports.hpp"

struct IPhotoCrawler;

struct ANALYZER_EXPORTS PhotoCrawlerBuilder
{
	PhotoCrawlerBuilder();
	virtual ~PhotoCrawlerBuilder();

	std::shared_ptr<IPhotoCrawler> build();
};

#endif
