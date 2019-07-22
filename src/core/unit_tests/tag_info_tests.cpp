
#include <gmock/gmock.h>

#include <QDate>

#include "tag.hpp"


TEST(TagInfoTest, fromTagNameInfoAndTagValueConstruction)
{
    const TagNameInfo tni(BaseTagsList::Date);
    const TagValue tv(QDate::currentDate());
    const Tag::Info ti(tni, tv);

    EXPECT_EQ(ti.name(), tni.getName());
    EXPECT_EQ(ti.displayName(), tni.getDisplayName());
    EXPECT_EQ(ti.getTypeInfo(), tni);
    EXPECT_EQ(ti.value(), tv);
}


TEST(TagInfoTest, fromPairConstruction)
{
    const TagNameInfo tni(BaseTagsList::Date);
    const TagValue tv(QDate::currentDate());
    const Tag::Info ti( std::make_pair(tni, tv) );

    EXPECT_EQ(ti.name(), tni.getName());
    EXPECT_EQ(ti.displayName(), tni.getDisplayName());
    EXPECT_EQ(ti.getTypeInfo(), tni);
    EXPECT_EQ(ti.value(), tv);
}
