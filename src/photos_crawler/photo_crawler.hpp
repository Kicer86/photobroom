
#ifndef ANALYZER_PHOTO_CRAWLER_HPP
#define ANALYZER_PHOTO_CRAWLER_HPP

#include <memory>

#include "iphoto_crawler.hpp"

struct IFileSystemScanner;
struct IAnalyzer;

class PHOTOS_CRAWLER_EXPORT PhotoCrawler: public IPhotoCrawler
{
    public:
        struct Impl;
        PhotoCrawler(std::unique_ptr<IFileSystemScanner> &&, std::unique_ptr<IAnalyzer> &&);
        virtual ~PhotoCrawler();

        virtual void crawl(const QStringList &, IMediaNotification *) override;
        virtual void stop() override;

    private:
        std::unique_ptr<Impl> m_impl;
};

#endif
