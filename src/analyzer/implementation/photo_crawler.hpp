
#ifndef ANALYZER_PHOTO_CRAWLER_HPP
#define ANALYZER_PHOTO_CRAWLER_HPP

#include <memory>

#include "iphoto_crawler.hpp"

struct IFileSystemScanner;
struct IAnalyzer;

class PhotoCrawler: public IPhotoCrawler
{
    public:
        PhotoCrawler(const std::shared_ptr<IFileSystemScanner> &, const std::shared_ptr<IAnalyzer> &);
        virtual ~PhotoCrawler();

        std::vector<std::string> crawl( const std::string& path ) override;
        virtual void setRules(const Rules &) override;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
};

#endif
