
#include <gmock/gmock.h>

#include "tag.hpp"
#include "base_tags.hpp"


TEST(TagNameInfoTest, emptyAfterConstruction)
{
    const TagNameInfo info;

    EXPECT_EQ(info.getName(), QString());
    EXPECT_EQ(info.getDisplayName(), QString());
    EXPECT_EQ(info.getType(), TagNameInfo::Type::Invalid);
    EXPECT_EQ(info.getTag(), BaseTagsList::Invalid);
}


TEST(TagNameInfoTest, copyOperation)
{
    const TagNameInfo info(BaseTagsList::Date);
    const TagNameInfo info2(info);

    TagNameInfo info3;
    info3 = info;

    EXPECT_EQ(info, info2);
    EXPECT_EQ(info, info3);
}


TEST(TagNameInfoTest, compareOperation)
{
    const auto tags = BaseTags::getAll();
    const std::size_t s = tags.size();

    for (std::size_t i = 0; i < s; i++)
        for (std::size_t j = 0; j < s; j++)
        {
            const TagNameInfo l(tags[i]);
            const TagNameInfo r(tags[j]);

            if (i == j)
            {
                EXPECT_TRUE(l == r);
                EXPECT_FALSE(l < r);
                EXPECT_FALSE(l > r);
            }
            else
            {
                EXPECT_FALSE(l == r);
                EXPECT_NE(l < r, l > r);
            }
        }
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
