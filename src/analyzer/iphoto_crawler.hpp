
#ifndef ANALYZER_IPHOTO_CRAWLER_HPP
#define ANALYZER_IPHOTO_CRAWLER_HPP

#include <vector>
#include <string>

#include "analyzer_exports.hpp"

struct ANALYZER_EXPORTS IPhotoCrawler
{
	virtual ~IPhotoCrawler() {}
	virtual void crawl( const std::vector<std::string> & ) = 0;
};

#endif
