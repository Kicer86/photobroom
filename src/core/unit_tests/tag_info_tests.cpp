
#include <gmock/gmock.h>

#include <QDate>

#include "tag.hpp"


TEST(TagInfoTest, fromPairConstruction)
{
    const TagNameInfo tni(TagTypes::Date);
    const TagValue tv(QDate::currentDate());
    const Tag::Info ti( std::make_pair(tni, tv) );

    EXPECT_EQ(ti.name(), tni.getName());
    EXPECT_EQ(ti.displayName(), tni.getDisplayName());
    EXPECT_EQ(ti.getTypeInfo(), tni);
    EXPECT_EQ(ti.value(), tv);
}


TEST(TagInfoTest, setter)
{
    const QDate d1 = QDate::fromString("2019.11.06", "yyyy.MM.dd");
    const QDate d2 = QDate::fromString("2119.03.02", "yyyy.MM.dd");

    const TagNameInfo tni(TagTypes::Date);
    const TagValue tv(d1);

    Tag::Info ti( std::make_pair(tni, tv) );

    ti.setValue(d2);
    EXPECT_NE(ti.value().getDate(), d1);
    EXPECT_EQ(ti.value().getDate(), d2);
}
