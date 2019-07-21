
#include <gmock/gmock.h>

#include "tag.hpp"


TEST(TagNameInfoTest, emptyAfterConstruction)
{
    TagNameInfo info;

    EXPECT_EQ(info.getName(), QString());
    EXPECT_EQ(info.getDisplayName(), QString());
    EXPECT_EQ(info.getType(), TagNameInfo::Type::Invalid);
    EXPECT_EQ(info.getTag(), BaseTagsList::Invalid);
}


typedef std::pair<BaseTagsList, TagNameInfo::Type> TagNameInfoExpectations;
struct TagNameInfoTest2: testing::TestWithParam<TagNameInfoExpectations> {};

TEST_P(TagNameInfoTest2, ProperValues)
{
    const TagNameInfo info(GetParam().first);

    EXPECT_NE(info.getName(),        QString());
    EXPECT_NE(info.getDisplayName(), QString());
    EXPECT_EQ(info.getType(),        GetParam().second);
    EXPECT_EQ(info.getTag(),         GetParam().first);
}

INSTANTIATE_TEST_CASE_P(ExtensionsTest,
                        TagNameInfoTest2,
                        testing::Values(
                            TagNameInfoExpectations{BaseTagsList::Event, TagNameInfo::Type::String },
                            TagNameInfoExpectations{BaseTagsList::Place, TagNameInfo::Type::String },
                            TagNameInfoExpectations{BaseTagsList::Date,  TagNameInfo::Type::Date   },
                            TagNameInfoExpectations{BaseTagsList::Time,  TagNameInfo::Type::Time   }
                        ),
);
