
#include <gtest/gtest.h>

#include "sql_filter_query_generator.hpp"

TEST(SqlFilterQueryGeneratorTest, HandlesEmptyList)
{
    Database::SqlFilterQueryGenerator generator;

    std::vector<Database::IFilter::Ptr> filters;
    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos", query);
    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesFlagsFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithFlags> filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filters.push_back(filter);

    filter->flags[Photo::FlagsE::ExifLoaded] = 1;
    QString query = generator.generate(filters);
    QString sql_query = generator.generate("all photos with flag tags_loaded = 1");
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos_id) WHERE flags.tags_loaded = '1'", query);
    EXPECT_EQ(sql_query, query);

    filter->flags.clear();
    filter->flags[Photo::FlagsE::Sha256Loaded] = 2;
    query = generator.generate(filters);
    sql_query = generator.generate("all photos with flag sha256_loaded = 2");
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos_id) WHERE flags.sha256_loaded = '2'", query);
    EXPECT_EQ(sql_query, query);

    filter->flags.clear();
    filter->flags[Photo::FlagsE::StagingArea] = 3;
    query = generator.generate(filters);
    sql_query = generator.generate("all photos with flag staging_area = 3");
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos_id) WHERE flags.staging_area = '3'", query);
    EXPECT_EQ(sql_query, query);

    filter->flags.clear();
    filter->flags[Photo::FlagsE::ThumbnailLoaded] = 4;
    query = generator.generate(filters);
    sql_query = generator.generate("all photos with flag thumbnail_loaded = 4");
    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos JOIN (flags) ON (flags.photo_id = photos_id) WHERE flags.thumbnail_loaded = '4'", query);
    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithTag> filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagNameInfo(BaseTagsList::Date), QString("test_value"));

    filters.push_back(filter);

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos with tag date = test_value");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos_id) "
              "WHERE tags.name = '3' AND tags.value = 'test_value'", query);
    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesFilterNotMatchingFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto sub_filter1 = std::make_shared<Database::FilterPhotosWithTag>(TagNameInfo(BaseTagsList::Time));
    auto filter = std::make_shared<Database::FilterNotMatchingFilter>(sub_filter1);
    filters.push_back(filter);

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos without tag time");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "WHERE photos.id NOT IN "
              "(SELECT photos.id AS photos_id FROM photos "
              "JOIN (tags) ON (tags.photo_id = photos_id) "
              "WHERE tags.name = '4')", query);

    EXPECT_EQ(query, sql_query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesSha256Filter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithSha256> filter = std::make_shared<Database::FilterPhotosWithSha256>();
    filters.push_back(filter);

    filter->sha256 = "1234567890";

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos with sha = '1234567890'");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (sha256sums) ON (sha256sums.photo_id = photos_id) "
              "WHERE sha256sums.sha256 = '1234567890'", query);

    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesIdFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithId> filter = std::make_shared<Database::FilterPhotosWithId>();
    filters.push_back(filter);

    filter->filter = Photo::Id(1234567890);

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos with id = '1234567890'");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos WHERE photos.id = '1234567890'", query);
    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesSimpleMergesWell)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    // sha256
    std::shared_ptr<Database::FilterPhotosWithSha256> sha_filter = std::make_shared<Database::FilterPhotosWithSha256>();
    filters.push_back(sha_filter);
    sha_filter->sha256 = "1234567890";

    //tag
    std::shared_ptr<Database::FilterPhotosWithTag> tag_filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagNameInfo(BaseTagsList::People), QString("test_value"));

    filters.push_back(tag_filter);

    //flags
    std::shared_ptr<Database::FilterPhotosWithFlags> flags_filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filters.push_back(flags_filter);
    flags_filter->flags[Photo::FlagsE::ExifLoaded] = 1;

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos with sha = '1234567890', tag people = test_value, flag tags_loaded = 1");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (flags, sha256sums, tags) "
              "ON (flags.photo_id = photos_id AND sha256sums.photo_id = photos_id AND tags.photo_id = photos_id) "
              "WHERE flags.tags_loaded = '1' AND sha256sums.sha256 = '1234567890' AND tags.name = '5' AND tags.value = 'test_value'", query);

    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagFiltersMergingWell)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    // #1 tag
    std::shared_ptr<Database::FilterPhotosWithTag> tag1_filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagNameInfo(BaseTagsList::Place), QString("test_value"));

    filters.push_back(tag1_filter);

    // #2 tag
    std::shared_ptr<Database::FilterPhotosWithTag> tag2_filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagNameInfo(BaseTagsList::Event), QString("test_value2"));

    filters.push_back(tag2_filter);

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos with tag event = test_value2, tag place = test_value");

    EXPECT_EQ("SELECT photos_id FROM "
              "(SELECT photos.id AS photos_id FROM photos "
                "JOIN (tags) ON (tags.photo_id = photos_id) "
                "WHERE tags.name = '2' AND tags.value = 'test_value') AS level_1_query "
              "JOIN (tags) ON (tags.photo_id = photos_id) "
              "WHERE tags.name = '1' AND tags.value = 'test_value2'", query);

    EXPECT_EQ(sql_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesSimpleOrFilters)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithFlags> flags = std::make_shared<Database::FilterPhotosWithFlags>();
    flags->flags[Photo::FlagsE::ExifLoaded] = 100;
    flags->flags[Photo::FlagsE::StagingArea] = 200;
    flags->mode = Database::FilterPhotosWithFlags::Mode::Or;

    filters.push_back(flags);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (flags) ON (flags.photo_id = photos_id) "
              "WHERE ( flags.staging_area = '200' OR flags.tags_loaded = '100' )", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesMergeOfIdFilterWithFlagsOne)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithFlags> flags = std::make_shared<Database::FilterPhotosWithFlags>();
    flags->flags[Photo::FlagsE::ExifLoaded] = 100;
    flags->flags[Photo::FlagsE::StagingArea] = 200;
    flags->mode = Database::FilterPhotosWithFlags::Mode::Or;

    auto id = std::make_shared<Database::FilterPhotosWithId>();
    id->filter = Photo::Id(1234567890);

    filters.push_back(flags);
    filters.push_back(id);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (flags) ON (flags.photo_id = photos_id) "
              "WHERE ( flags.staging_area = '200' OR flags.tags_loaded = '100' ) AND photos.id = '1234567890'", query);
}


TEST(SqlFilterQueryGeneratorTest, SimpleFilterPhotosMatchingExpression)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    const SearchExpressionEvaluator::Expression expression = { {"Person 1", false} };
    std::shared_ptr<Database::FilterPhotosMatchingExpression> filter = std::make_shared<Database::FilterPhotosMatchingExpression>( expression );

    filters.push_back(filter);

    const QString query = generator.generate(filters);
    const QString sql_query = generator.generate("all photos with any tag ~ 'Person 1'");

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (tags) ON (tags.photo_id = photos_id) "
              "WHERE tags.value LIKE '%Person 1%'", query);

    EXPECT_EQ(query, sql_query);
}


TEST(SqlFilterQueryGeneratorTest, FilterPhotosMatchingDoubleExpression)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    const SearchExpressionEvaluator::Expression expression = { {"Person 1", false}, {"Person 2", false} };
    std::shared_ptr<Database::FilterPhotosMatchingExpression> filter = std::make_shared<Database::FilterPhotosMatchingExpression>(expression);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "JOIN (tags) ON (tags.photo_id = photos_id) "
              "WHERE (tags.value LIKE '%Person 1%' OR tags.value LIKE '%Person 2%')", query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByRegularRole)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::Regular);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "WHERE photos.id NOT IN "
              "("
                    "SELECT groups_members.photo_id FROM groups_members "
                    "UNION "
                    "SELECT groups.representative_id FROM groups"
              ")", query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByGroupRepresentativeRole)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::GroupRepresentative);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "WHERE photos.id IN "
              "("
                    "SELECT groups.representative_id FROM groups"
              ")", query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByGroupMemberRole)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::GroupMember);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id AS photos_id FROM photos "
              "WHERE photos.id IN "
              "("
                    "SELECT groups_members.photo_id FROM groups_members"
              ")", query);
}
