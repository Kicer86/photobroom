
#include "photo_crawler.hpp"

#include "ifile_system_scanner.hpp"
#include "ifile_analyzer.hpp"

struct PhotoCrawler::Impl
{
	Impl(IFileSystemScanner *scanner, IFileAnalyzer *analyzer): m_scanner(scanner), m_analyzer(analyzer) {}
	virtual ~Impl() {}

	std::unique_ptr<IFileSystemScanner> m_scanner;
	std::unique_ptr<IFileAnalyzer> m_analyzer;
};

PhotoCrawler::PhotoCrawler(IFileSystemScanner *scanner, IFileAnalyzer *analyzer): m_impl(new Impl(scanner, analyzer))
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
