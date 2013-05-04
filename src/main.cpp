
#include <iostream>
#include <vector>
#include <string>

#include "analyzer/photo_crawler_builder.hpp"
#include "analyzer/iphoto_crawler.hpp"

int main(int argc, char **argv)
{
	std::shared_ptr<IPhotoCrawler> crawler = PhotoCrawlerBuilder().build();

	std::string path = "/home/michal/projects/sources/broom/Asia i kot";
	std::vector<std::string> paths;

	paths.push_back(path);

	crawler->crawl(paths);

    return 0;
}
