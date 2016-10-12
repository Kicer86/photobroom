
#include <gtest/gtest.h>

#include <core/base_tags.hpp>

#include "database_tools/tag_info_collector.hpp"

TEST(TagInfoCollectorTest, isConstructible)
{
    EXPECT_NO_THROW(
    {
        TagInfoCollector tagInfoCollector;
    });
}


TEST(TagInfoCollectorTest, getsWithoutDatabase)
{
    TagInfoCollector tagInfoCollector;

    std::vector<BaseTagsList> tags = BaseTags::getAll();

    for(const BaseTagsList& tag: tags)
    {
        const std::set<TagValue>& values = tagInfoCollector.get( TagNameInfo(BaseTagsList::Event) );

        EXPECT_EQ(values.empty(), true);
    }
}

