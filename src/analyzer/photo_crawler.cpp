
#include "photo_crawler.hpp"

namespace
{
	struct Crawler: public IPhotoCrawler
	{
		void crawl(const std::vector<std::string> &paths)
		{

		}
	};
}


IPhotoCrawler* PhotoCrawlerFactory::get()
{
	return nullptr;
}
