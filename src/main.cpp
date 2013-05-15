
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>

#include "analyzer/photo_crawler_builder.hpp"
#include "analyzer/iphoto_crawler.hpp"
#include "database/idatabase.hpp"
#include "database/databasebuilder.hpp"

int main(int argc, char **argv)
{
    std::shared_ptr<IPhotoCrawler> crawler = PhotoCrawlerBuilder().build();

    std::string path = "/home/michal/projects/sources/broom/Asia i kot";
    std::vector<std::string> paths;

    paths.push_back(path);

    std::vector<std::string> files = crawler->crawl(paths);
    
    Database::IDatabaseFrontend *database = Database::Builder().get();

    for (auto f: files)
        database->addFile(f, Database::IDatabaseFrontend::Description());

    return 0;
}
