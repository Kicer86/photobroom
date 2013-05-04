
#ifndef ANALYZER_PHOTO_CRAWLER_HPP
#define ANALYZER_PHOTO_CRAWLER_HPP

#include <memory>

#include "iphoto_crawler.hpp"

struct IFileSystemScanner;
struct IAnalyzer;

class PhotoCrawler: public IPhotoCrawler
{
    public:
        PhotoCrawler(IFileSystemScanner *, IAnalyzer *);
        virtual ~PhotoCrawler();
        
        virtual void crawl( const std::vector<std::string> & ) override;
		virtual void setRules(const Rules &) override;

	private: 
		struct Impl;
		std::unique_ptr<Impl> m_impl;
};

#endif
