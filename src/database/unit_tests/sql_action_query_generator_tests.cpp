
#include <gtest/gtest.h>

#include "sql_action_query_generator.hpp"


TEST(SqlActionQueryGeneratorTest, HandlesModifyFlagsAction)
{
    Database::SqlActionQueryGenerator generator;

    auto action = std::make_shared<Database::ModifyFlagAction>();

    action->flags[Photo::FlagsE::ExifLoaded] = 1;
    QString query = generator.generate(action);
    EXPECT_EQ("UPDATE flags SET tags_loaded = 1 WHERE photo_id IN (%3)", query);

    action->flags.clear();
    action->flags[Photo::FlagsE::Sha256Loaded] = 2;
    query = generator.generate(action);
    EXPECT_EQ("UPDATE flags SET sha256_loaded = 2 WHERE photo_id IN (%3)", query);

    action->flags.clear();
    action->flags[Photo::FlagsE::StagingArea] = 3;
    query = generator.generate(action);
    EXPECT_EQ("UPDATE flags SET staging_area = 3 WHERE photo_id IN (%3)", query);

    action->flags.clear();
    action->flags[Photo::FlagsE::ThumbnailLoaded] = 4;
    query = generator.generate(action);
    EXPECT_EQ("UPDATE flags SET thumbnail_loaded = 4 WHERE photo_id IN (%3)", query);
}
