
#include <gmock/gmock.h>

#include "tag.hpp"
#include "base_tags.hpp"


TEST(TagNameInfoTest, emptyAfterConstruction)
{
    const TagTypeInfo info;

    EXPECT_EQ(info.getName(), QString());
    EXPECT_EQ(info.getDisplayName(), QString());
    EXPECT_EQ(info.getTag(), Tag::Types::Invalid);
}


TEST(TagNameInfoTest, copyOperation)
{
    const TagTypeInfo info(Tag::Types::Date);
    const TagTypeInfo info2(info);

    TagTypeInfo info3;
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
            const TagTypeInfo l(tags[i]);
            const TagTypeInfo r(tags[j]);

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

typedef std::pair<Tag::Types, Tag::ValueType> TagNameInfoExpectations;
struct TagNameInfoTest2: testing::TestWithParam<TagNameInfoExpectations> {};

TEST_P(TagNameInfoTest2, ProperValues)
{
    const TagTypeInfo info(GetParam().first);

    EXPECT_NE(info.getName(),                   QString());
    EXPECT_NE(info.getDisplayName(),            QString());
    EXPECT_EQ(BaseTags::getType(info.getTag()), GetParam().second);
    EXPECT_EQ(info.getTag(),                    GetParam().first);
}

INSTANTIATE_TEST_CASE_P(ExtensionsTest,
                        TagNameInfoTest2,
                        testing::Values(
                            TagNameInfoExpectations{Tag::Types::Event, Tag::ValueType::String },
                            TagNameInfoExpectations{Tag::Types::Place, Tag::ValueType::String },
                            TagNameInfoExpectations{Tag::Types::Date,  Tag::ValueType::Date   },
                            TagNameInfoExpectations{Tag::Types::Time,  Tag::ValueType::Time   }
                        )
);
