
#include "photo_crawler.hpp"

#include "ifile_system_scanner.hpp"
#include "ifile_analyzer.hpp"

struct PhotoCrawler::Impl
{
	Impl(IFileSystemScanner *scanner): m_scanner(scanner) {}
	virtual ~Impl() {}

	IFileSystemScanner *m_scanner;
};

PhotoCrawler::PhotoCrawler(IFileSystemScanner *scanner, IFileAnalyzer *analyzer): m_impl(new Impl(scanner))
{

}


PhotoCrawler::~PhotoCrawler()
{

}


void PhotoCrawler::crawl(const std::vector< std::string > &paths)
{
	for(auto path: paths)
	{
		std::vector<std::string> files = m_impl->m_scanner->getFilesFor(path);

	}
}
