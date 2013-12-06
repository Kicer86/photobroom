
#include <memory>

#include <gtest/gtest.h>

#include "photo_crawler_builder.hpp"


TEST(PhotoCrawlerBuilderShould, returnPhotoCrawler)
{
    PhotoCrawlerBuilder builder;
    
    std::shared_ptr<IPhotoCrawler> crawler = builder.build();
    
    ASSERT_NE(crawler.get(), nullptr);
}
