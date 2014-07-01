
#ifndef ANALYZER_PHOTO_CRAWLER_HPP
#define ANALYZER_PHOTO_CRAWLER_HPP

#include <memory>

#include "iphoto_crawler.hpp"

struct IFileSystemScanner;
struct IAnalyzer;

class PhotoCrawler: public IPhotoCrawler
{
    public:
        struct Impl;
        PhotoCrawler(const std::shared_ptr<IFileSystemScanner> &, const std::shared_ptr<IAnalyzer> &);
        virtual ~PhotoCrawler();

        virtual void crawl(const std::string &, IMediaNotification *) override;
        virtual void setRules(const Rules &) override;

    private:
        std::unique_ptr<Impl> m_impl;
};

#endif
