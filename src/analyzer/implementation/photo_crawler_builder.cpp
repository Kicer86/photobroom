
#include "photo_crawler_builder.hpp"

PhotoCrawlerBuilder::PhotoCrawlerBuilder()
{
}

PhotoCrawlerBuilder::~PhotoCrawlerBuilder()
{
}

std::shared_ptr<IPhotoCrawler> PhotoCrawlerBuilder::build()
{
	return std::shared_ptr<IPhotoCrawler> (0);
}
