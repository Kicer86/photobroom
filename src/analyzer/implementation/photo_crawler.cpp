
#include "photo_crawler.hpp"

#include "ifile_system_scanner.hpp"

struct PhotoCrawler::Impl
{
	Impl() {}
	virtual ~Impl() {}
};

PhotoCrawler::PhotoCrawler(IFileSystemScanner *scanner)
{

}


PhotoCrawler::~PhotoCrawler()
{

}


void PhotoCrawler::crawl(const std::vector< std::string >& )
{

}
