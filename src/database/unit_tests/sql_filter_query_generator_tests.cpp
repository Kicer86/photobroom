
#include <gtest/gtest.h>
#include <QTime>

#include "sql_filter_query_generator.hpp"


TEST(SqlFilterQueryGeneratorTest, HandlesEmptyList)
{
    Database::SqlFilterQueryGenerator generator;

    Database::EmptyFilter filter;
    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT id FROM photos", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesFlagsFilter)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithFlags filter;

    filter.flags[Photo::FlagsE::ExifLoaded] = 1;
    filter.comparison[Photo::FlagsE::ExifLoaded] = Database::ValueMode::Less;
    QString query = generator.generate(filter);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.tags_loaded < '1'", query);

    filter.flags.clear();
    filter.flags[Photo::FlagsE::Sha256Loaded] = 2;
    filter.comparison[Photo::FlagsE::Sha256Loaded] = Database::ValueMode::Greater;
    query = generator.generate(filter);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.sha256_loaded > '2'", query);

    filter.flags.clear();
    filter.flags[Photo::FlagsE::StagingArea] = 3;
    query = generator.generate(filter);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.staging_area = '3'", query);

    filter.flags.clear();
    filter.flags[Photo::FlagsE::ThumbnailLoaded] = 4;
    query = generator.generate(filter);
    EXPECT_EQ("SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE flags.thumbnail_loaded = '4'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilter)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Date, QString("test_value"));

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '3' AND tags.value = 'test_value'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithEmptyValue)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Time);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithTagValueCasting)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Rating, 5, Database::ValueMode::Equal);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '6' AND CAST(tags.value AS INTEGER) = '5'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithComparisonModeSetToEqual)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Time, QTime(12,34), Database::ValueMode::Equal);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4' AND tags.value = '12:34:00'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithComparisonModeSetToGreater)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Time, QTime(12,34), Database::ValueMode::Greater);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4' AND tags.value > '12:34:00'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithComparisonModeSetToGreaterOrEqual)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Time, QTime(12,34), Database::ValueMode::GreaterOrEqual);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4' AND tags.value >= '12:34:00'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithComparisonModeSetToLess)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Time, QTime(12,34), Database::ValueMode::Less);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4' AND tags.value < '12:34:00'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesTagsFilterWithComparisonModeSetToLessOrEqual)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithTag filter(TagTypes::Time, QTime(12,34), Database::ValueMode::LessOrEqual);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (tags) "
              "ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4' AND tags.value <= '12:34:00'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesFilterNotMatchingFilter)
{
    Database::SqlFilterQueryGenerator generator;

    Database::FilterPhotosWithTag sub_filter1(TagTypes::Time);
    Database::FilterNotMatchingFilter filter = Database::Filter(sub_filter1);

    const QString query = generator.generate(Database::Filter(filter));
    EXPECT_EQ("SELECT id FROM photos "
              "WHERE id NOT IN "
              "(SELECT photos.id FROM photos "
              "JOIN (tags) ON (tags.photo_id = photos.id) "
              "WHERE tags.name = '4')", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesSha256Filter)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithSha256 filter;

    filter.sha256 = "1234567890";

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT id FROM photos "
              "JOIN (sha256sums) ON (sha256sums.photo_id = photos.id) "
              "WHERE sha256sums.sha256 = '1234567890'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesIdFilter)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithId filter;

    filter.filter = Photo::Id(1234567890);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT id FROM photos WHERE id = '1234567890'", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesSimpleMergesWell)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::Filter> filters;

    // sha256
    Database::FilterPhotosWithSha256 sha_filter;
    sha_filter.sha256 = "1234567890";
    filters.push_back(sha_filter);

    //tag
    Database::FilterPhotosWithTag tag_filter(TagTypes::Place, QString("place 1"));
    filters.push_back(tag_filter);

    //flags
    Database::FilterPhotosWithFlags flags_filter;
    flags_filter.flags[Photo::FlagsE::ExifLoaded] = 1;

    filters.push_back(flags_filter);

    const QString query = generator.generate(Database::GroupFilter(filters));

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
            "WHERE tags.name = '2' AND tags.value = 'place 1'"
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

    // #1 tag
    Database::FilterPhotosWithTag tag1_filter(TagTypes::Place, QString("test_value"));

    // #2 tag
    Database::FilterPhotosWithTag tag2_filter(TagTypes::Event, QString("test_value2"));

    Database::GroupFilter all_filters = {tag1_filter, tag2_filter};
    const QString query = generator.generate(all_filters);

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
    std::vector<Database::Filter> filters;

    Database::FilterPhotosWithFlags flags;
    flags.flags[Photo::FlagsE::ExifLoaded] = 100;
    flags.flags[Photo::FlagsE::StagingArea] = 200;
    flags.mode = Database::FilterPhotosWithFlags::Mode::Or;

    filters.push_back(flags);

    const QString query = generator.generate(filters);

    EXPECT_EQ("SELECT photos.id FROM photos "
              "JOIN (flags) ON (flags.photo_id = photos.id) "
              "WHERE ( flags.staging_area = '200' OR flags.tags_loaded = '100' )", query);
}


TEST(SqlFilterQueryGeneratorTest, HandlesMergeOfIdFilterWithFlagsOne)
{
    Database::SqlFilterQueryGenerator generator;
    std::vector<Database::Filter> filters;

    Database::FilterPhotosWithFlags flags;
    flags.flags[Photo::FlagsE::ExifLoaded] = 100;
    flags.comparison[Photo::FlagsE::ExifLoaded] = Database::ValueMode::GreaterOrEqual;
    flags.flags[Photo::FlagsE::StagingArea] = 200;
    flags.comparison[Photo::FlagsE::StagingArea] = Database::ValueMode::LessOrEqual;
    flags.mode = Database::FilterPhotosWithFlags::Mode::Or;

    Database::FilterPhotosWithId id;
    id.filter = Photo::Id(1234567890);

    filters.push_back(flags);
    filters.push_back(id);

    const QString query = generator.generate(Database::GroupFilter(filters));

    const QString expected_query =
        "SELECT id FROM photos WHERE "
        "id IN "
        "("
            "SELECT photos.id FROM photos JOIN (flags) ON (flags.photo_id = photos.id) WHERE ( flags.staging_area <= '200' OR flags.tags_loaded >= '100' )"
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

    const SearchExpressionEvaluator::Expression expression = { {"Person 1", false} };
    Database::FilterPhotosMatchingExpression filter( expression );

    const QString query = generator.generate(filter);

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

    const SearchExpressionEvaluator::Expression expression = { {"Person 1", false}, {"Person 2", false} };
    Database::FilterPhotosMatchingExpression filter(expression);

    const QString query = generator.generate(filter);

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
    Database::FilterPhotosWithRole filter(Database::FilterPhotosWithRole::Role::Regular);

    const QString query = generator.generate(filter);

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
    Database::FilterPhotosWithRole filter(Database::FilterPhotosWithRole::Role::GroupRepresentative);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT groups.representative_id FROM groups", query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByGroupMemberRole)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithRole filter(Database::FilterPhotosWithRole::Role::GroupMember);

    const QString query = generator.generate(filter);

    EXPECT_EQ("SELECT groups_members.photo_id FROM groups_members", query);
}


TEST(SqlFilterQueryGeneratorTest, FiltersPhotosByGeneralFlags)
{
    Database::SqlFilterQueryGenerator generator;
    Database::FilterPhotosWithGeneralFlag filter("some_name", 12345);

    const QString query = generator.generate(filter);

    EXPECT_EQ(query, "SELECT photos.id FROM photos LEFT JOIN (general_flags) ON (general_flags.photo_id = photos.id AND general_flags.name = 'some_name') WHERE COALESCE(general_flags.value, 0) = 12345");
}
