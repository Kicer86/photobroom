
#include "photo_crawler.hpp"

#include "ifile_system_scanner.hpp"
#include "ianalyzer.hpp"

struct PhotoCrawler::Impl
{
    Impl(const std::shared_ptr<IFileSystemScanner>& scanner,
         const std::shared_ptr<IAnalyzer>& analyzer): m_scanner(scanner), m_analyzer(analyzer) {}
    ~Impl() {}

    std::shared_ptr<IFileSystemScanner> m_scanner;
    std::shared_ptr<IAnalyzer> m_analyzer;
};


PhotoCrawler::PhotoCrawler(const std::shared_ptr<IFileSystemScanner>& scanner,
                           const std::shared_ptr<IAnalyzer>& analyzer): m_impl(new Impl(scanner, analyzer))
{

}


PhotoCrawler::~PhotoCrawler()
{

}


void PhotoCrawler::crawl(const std::string& path, IMediaNotification* notifications)
{
    std::vector<std::string> files = m_impl->m_scanner->getFilesFor(path);

    for(const auto& file: files)
        if (m_impl->m_analyzer->isImage(file))
            notifications->found(file);

}


void PhotoCrawler::setRules(const Rules &)
{

}
