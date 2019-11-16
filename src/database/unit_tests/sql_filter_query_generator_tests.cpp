
#include <gtest/gtest.h>

#include "sql_filter_query_generator.hpp"

TEST(SqlFilterQueryGeneratorTest, HandlesEmptyList)
{
    Database::SqlFilterQueryGenerator generator;

    std::vector<Database::IFilter::Ptr> filters;
    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT id FROM photos", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesFlagsFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithFlags> filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filters.push_back(filter);

    filter->flags[Photo::FlagsE::ExifLoaded] = 1;
    QString query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.tags_loaded = '1'", query);

    filter->flags.clear();
    filter->flags[Photo::FlagsE::Sha256Loaded] = 2;
    query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.sha256_loaded = '2'", query);

    filter->flags.clear();
    filter->flags[Photo::FlagsE::StagingArea] = 3;
    query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.staging_area = '3'", query);

    filter->flags.clear();
    filter->flags[Photo::FlagsE::ThumbnailLoaded] = 4;
    query = generator.generate(filters);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.thumbnail_loaded = '4'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithTag> filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagTypeInfo(TagTypes::Date), QString("test_value"));

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '3' AND tags.value = 'test_value'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesFilterNotMatchingFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto sub_filter1 = std::make_shared<Database::FilterPhotosWithTag>(TagTypeInfo(TagTypes::Time));
    auto filter = std::make_shared<Database::FilterNotMatchingFilter>(sub_filter1);
    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT id FROM photos "
              "WHERE id NOT IN "
              "(SELECT photos.id FROM photos "
              "JOIN (tags) ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4')", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesSha256Filter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithSha256> filter = std::make_shared<Database::FilterPhotosWithSha256>();
    filters.push_back(filter);

    filter->sha256 = "1234567890";

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT id FROM photos "
              "JOIN (sha256sums) ON (sha256sums.photo_id = photos.id) "
              "WHERE sha256sums.sha256 = '1234567890'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesIdFilter)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    std::shared_ptr<Database::FilterPhotosWithId> filter = std::make_shared<Database::FilterPhotosWithId>();
    filters.push_back(filter);

    filter->filter = Photo::Id(1234567890);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT id FROM photos WHERE id = '1234567890'", query);
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
        std::make_shared<Database::FilterPhotosWithTag>(TagTypeInfo(TagTypes::_People), QString("test_value"));

    filters.push_back(tag_filter);

    //flags
    std::shared_ptr<Database::FilterPhotosWithFlags> flags_filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filters.push_back(flags_filter);
    flags_filter->flags[Photo::FlagsE::ExifLoaded] = 1;

    const QString query = generator.generate(filters);

    const QString expected_query =
        "SELECT id FROM photos "
        "WHERE id IN "
        "("
            "SELECT id FROM photos JOIN (sha256sums) ON (sha256sums.photo_id = photos.id) "
            "WHERE sha256sums.sha256 = '1234567890'"
        ") "
        "AND id IN "
        "("
            "SELECT photos.id FROM photos JOIN (tags) ON (tags.photo_id = photos.id) "
            "WHERE tags.name = '5' AND tags.value = 'test_value'"
        ") "
        "AND id IN "
        "("
            "SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) "
            "WHERE flags.tags_loaded = '1'"
        ")";

    EXPECT_EQ(expected_query, query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagFiltersMergingWell)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    // #1 tag
    std::shared_ptr<Database::FilterPhotosWithTag> tag1_filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagTypeInfo(TagTypes::Place), QString("test_value"));

    filters.push_back(tag1_filter);

    // #2 tag
    std::shared_ptr<Database::FilterPhotosWithTag> tag2_filter =
        std::make_shared<Database::FilterPhotosWithTag>(TagTypeInfo(TagTypes::Event), QString("test_value2"));

    filters.push_back(tag2_filter);

    const QString query = generator.generate(filters);

    const QString expected_query =
        "SELECT id FROM photos "
        "WHERE id IN "
        "("
            "SELECT photos.id FROM photos JOIN (tags) ON (tags.photo_id = photos.id) "
            "WHERE tags.name = '2' AND tags.value = 'test_value'"
        ") "
        "AND id IN "
        "("
            "SELECT photos.id FROM photos JOIN (tags) ON (tags.photo_id = photos.id) "
            "WHERE tags.name = '1' AND tags.value = 'test_value2'"
        ")";

    EXPECT_EQ(expected_query, query);
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

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (flags) ON (flags.photo_id = photos.id) "
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

    const QString expected_query =
        "SELECT id FROM photos WHERE "
        "id IN "
        "("
            "SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE ( flags.staging_area = '200' OR flags.tags_loaded = '100' )"
        ") "
        "AND id IN "
        "("
            "SELECT id FROM photos WHERE id = '1234567890'"
        ")";

    EXPECT_EQ(expected_query, query);
}


TEST(SqlFilterQueryGeneratorTest, SimpleFilterPhotosMatchingExpression)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    const SearchExpressionEvaluator::Expression expression = { {"Person 1", false} };
    std::shared_ptr<Database::FilterPhotosMatchingExpression> filter = std::make_shared<Database::FilterPhotosMatchingExpression>( expression );

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    const QString expected_query =
        "SELECT photos.id FROM photos "
        "WHERE photos.id IN "
        "("
            "SELECT photos.id FROM photos JOIN (tags) ON (photos.id = tags.photo_id) WHERE (tags.value LIKE '%Person 1%')"
        ") "
        "OR photos.id IN "
        "("
            "SELECT photos.id FROM photos JOIN (people, people_names) ON (photos.id = people.photo_id AND people.person_id = people_names.id) WHERE (people_names.name LIKE '%Person 1%')"
        ")";

    EXPECT_EQ(expected_query, query);
}


TEST(SqlFilterQueryGeneratorTest, FilterPhotosMatchingDoubleExpression)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    const SearchExpressionEvaluator::Expression expression = { {"Person 1", false}, {"Person 2", false} };
    std::shared_ptr<Database::FilterPhotosMatchingExpression> filter = std::make_shared<Database::FilterPhotosMatchingExpression>(expression);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    const QString expected_query =
        "SELECT photos.id FROM photos "
        "WHERE photos.id IN "
        "("
            "SELECT photos.id FROM photos JOIN (tags) ON (photos.id = tags.photo_id) WHERE (tags.value LIKE '%Person 1%' OR tags.value LIKE '%Person 2%')"
        ") "
        "OR photos.id IN "
        "("
            "SELECT photos.id FROM photos JOIN (people, people_names) ON (photos.id = people.photo_id AND people.person_id = people_names.id) WHERE (people_names.name LIKE '%Person 1%' OR people_names.name LIKE '%Person 2%')"
        ")";

    EXPECT_EQ(expected_query, query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByRegularRole)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::Regular);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT id FROM photos "
              "WHERE id NOT IN "
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

    EXPECT_EQ("SELECT groups.representative_id FROM groups", query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByGroupMemberRole)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::IFilter::Ptr> filters;

    auto filter = std::make_shared<Database::FilterPhotosWithRole>(Database::FilterPhotosWithRole::Role::GroupMember);

    filters.push_back(filter);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT groups_members.photo_id FROM groups_members", query);
}
