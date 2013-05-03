
#include "photo_crawler_builder.hpp"

#include "photo_crawler.hpp"
#include "file_analyzer.hpp"
#include "filesystemscanner.hpp"

PhotoCrawlerBuilder::PhotoCrawlerBuilder()
{
}

PhotoCrawlerBuilder::~PhotoCrawlerBuilder()
{
}

std::shared_ptr<IPhotoCrawler> PhotoCrawlerBuilder::build()
{
	PhotoCrawler *crawler = new PhotoCrawler(new FileSystemScanner, new FileAnalyzer);

	return std::shared_ptr<IPhotoCrawler> (crawler);
}
