
#ifndef ANALYZER_PHOTO_CRAWLER_HPP
#define ANALYZER_PHOTO_CRAWLER_HPP

#include "iphoto_crawler.hpp"


class PhotoCrawler: public IPhotoCrawler
{
    public:
        PhotoCrawler();
        virtual ~PhotoCrawler();
        
        virtual void crawl( const std::vector<std::string> & ) override;
};

#endif
