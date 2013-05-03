
#ifndef ANALYZER_PHOTO_CRAWLER_HPP
#define ANALYZER_PHOTO_CRAWLER_HPP

#include <memory>

#include "iphoto_crawler.hpp"

struct IFileSystemScanner;
struct IFileAnalyzer;

class PhotoCrawler: public IPhotoCrawler
{
    public:
        PhotoCrawler(IFileSystemScanner *, IFileAnalyzer *);
        virtual ~PhotoCrawler();
        
        virtual void crawl( const std::vector<std::string> & ) override;

	private: 
		struct Impl;
		std::unique_ptr<Impl> m_impl;
};

#endif
