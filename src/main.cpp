
#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "analyzer/photo_crawler_builder.hpp"
#include "analyzer/iphoto_crawler.hpp"
#include "database/idatabase.hpp"
#include "database/databasebuilder.hpp"
#include "gui/gui.hpp"

int main(int argc, char **argv)
{
    std::shared_ptr<IPhotoCrawler> crawler = PhotoCrawlerBuilder().build();

    std::string path = "Asia i kot";
    std::vector<std::string> paths;

    paths.push_back(path);

    std::vector<std::string> files = crawler->crawl(paths);
    
    Database::IFrontend *database = Database::Builder().get();

    for (auto f: files)
        database->addFile(f, Database::IFrontend::Description());

    std::shared_ptr<Gui::IUi> gui = Gui::Factory::get();

    gui->init(argc, argv);
    gui->run();
    gui->quit();

    return 0;
}
