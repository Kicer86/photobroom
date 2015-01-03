
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


TEST(SqlSelectQueryGeneratorTest, HandlesTagsFilter)
{
    Database::SqlSelectQueryGenerator generator;
    std::deque<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithTag> filter = std::make_shared<Database::FilterPhotosWithTag>();
    filters.push_back(filter);

    filter->tagName = TagNameInfo("test_name", TagNameInfo::Text);
    filter->tagValue = "test_value";

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (tags, tag_names) "
              "ON (tags.photo_id = photos.id AND tags.name_id = tag_names.id) "
              "WHERE tag_names.name = 'test_name' AND tags.value = 'test_value'", query);
}


TEST(SqlSelectQueryGeneratorTest, HandlesWithoutTagsFilter)
{
    Database::SqlSelectQueryGenerator generator;
    std::deque<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithoutTag> filter = std::make_shared<Database::FilterPhotosWithoutTag>();
    filters.push_back(filter);

    filter->tagName = TagNameInfo("test_name", TagNameInfo::Text);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "WHERE photos.id NOT IN "
              "(SELECT tags.photo_id FROM tags "
              "JOIN tag_names ON ( tag_names.id = tags.name_id) "
              "WHERE tag_names.name = 'test_name')", query);
}


TEST(SqlSelectQueryGeneratorTest, HandlesSha256Filter)
{
    Database::SqlSelectQueryGenerator generator;
    std::deque<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithSha256> filter = std::make_shared<Database::FilterPhotosWithSha256>();
    filters.push_back(filter);

    filter->sha256 = "1234567890";

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (hashes) ON (hashes.photo_id = photos.id) "
              "WHERE hashes.hash = '1234567890'", query);
}


TEST(SqlSelectQueryGeneratorTest, HandlesMergesWell)
{
    Database::SqlSelectQueryGenerator generator;
    std::deque<Database::IFilter::Ptr> filters;

    // sha256
    std::shared_ptr<Database::FilterPhotosWithSha256> sha_filter = std::make_shared<Database::FilterPhotosWithSha256>();
    filters.push_back(sha_filter);
    sha_filter->sha256 = "1234567890";

    //tag
    std::shared_ptr<Database::FilterPhotosWithTag> tag_filter = std::make_shared<Database::FilterPhotosWithTag>();
    filters.push_back(tag_filter);
    tag_filter->tagName = TagNameInfo("test_name", TagNameInfo::Text);
    tag_filter->tagValue = "test_value";

    //flags
    std::shared_ptr<Database::FilterPhotosWithFlags> flags_filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filters.push_back(flags_filter);
    flags_filter->flag = IPhotoInfo::FlagsE::ExifLoaded;
    flags_filter->value = 1;

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (tags, tag_names, flags, hashes) "
              "ON (tags.photo_id = photos.id AND tags.name_id = tag_names.id AND flags.photo_id = photos.id AND hashes.photo_id = photos.id) "
              "WHERE hashes.hash = '1234567890' AND tag_names.name = 'test_name' AND tags.value = 'test_value' AND flags.tags_loaded = '1'", query);
}
