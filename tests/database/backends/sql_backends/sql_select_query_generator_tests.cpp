
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
    filters.push_back(filter);

    filter->flag = IPhotoInfo::FlagsE::ExifLoaded;
    filter->value = 1;
    QString query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.tags_loaded = '1'", query);

    filter->flag = IPhotoInfo::FlagsE::Sha256Loaded;
    filter->value = 2;
    query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.hash_loaded = '2'", query);

    filter->flag = IPhotoInfo::FlagsE::StagingArea;
    filter->value = 3;
    query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.staging_area = '3'", query);

    filter->flag = IPhotoInfo::FlagsE::ThumbnailLoaded;
    filter->value = 4;
    query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.thumbnail_loaded = '4'", query);
}
