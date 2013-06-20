
#ifndef ANALYZER_IPHOTO_CRAWLER_HPP
#define ANALYZER_IPHOTO_CRAWLER_HPP

#include <vector>
#include <string>

#include "analyzer_export.h"

struct Rules;

struct ANALYZER_EXPORT IPhotoCrawler
{
	virtual ~IPhotoCrawler() {}

	virtual std::vector<std::string> crawl(const std::vector<std::string> &) = 0;   //return list of media files for given list of paths
	virtual void setRules(const Rules &) = 0;                                       //provide crawl rules
};

#endif
