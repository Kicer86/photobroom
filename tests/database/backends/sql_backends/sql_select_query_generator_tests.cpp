
#include <gtest/gtest.h>

#include "sql_select_query_generator.hpp"

TEST(SqlSelectQueryGeneratorTest, HandlesEmptyList)
{
    Database::SqlSelectQueryGenerator generator;

    std::deque<Database::IFilter::Ptr> filters;
    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos", query);
}


TEST(SqlSelectQueryGeneratorTest, HandlesFalgsFilter)
{
    Database::SqlSelectQueryGenerator generator;
    std::deque<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithFlags> filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filter->flag = IPhotoInfo::FlagsE::ExifLoaded;
    filter->value = 1;

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.tags_loaded = '1'", query);
}
