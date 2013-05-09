
#include "photo_crawler_builder.hpp"

#include "photo_crawler.hpp"
#include "file_analyzer.hpp"
#include "filesystemscanner.hpp"
#include "default_analyzers/ext_jpeganalyzer.hpp"

PhotoCrawlerBuilder::PhotoCrawlerBuilder()
{
}

PhotoCrawlerBuilder::~PhotoCrawlerBuilder()
{
}

std::shared_ptr<IPhotoCrawler> PhotoCrawlerBuilder::build()
{
    FileAnalyzer *analyzer = new FileAnalyzer;
    
    //add subanalyzers
    analyzer->registerAnalyzer(new Ext_DefaultAnalyzer);
    
    //build crawler
	PhotoCrawler *crawler = new PhotoCrawler(new FileSystemScanner, analyzer);

	return std::shared_ptr<IPhotoCrawler> (crawler);
}
