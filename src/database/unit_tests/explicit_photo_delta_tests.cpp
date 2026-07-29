
#include <gmock/gmock.h>

#include "explicit_photo_delta.hpp"


using namespace Photo;

TEST(ExplicitPhotoTest, dataFetch)
{
    using ED = ExplicitDelta<Photo::Field::Tags, Photo::Field::Geometry>;

    DataDelta data(Photo::Id(4));
    data.insert<Photo::Field::Tags>({});
    data.insert<Photo::Field::Geometry>(QSize(1, 2));

    ED ed(data);

    EXPECT_EQ(ed.get<Photo::Field::Geometry>(), QSize(1, 2));
}

TEST(ExplicitPhotoTest, allFieldsContainsEveryPhotoField)
{
    EXPECT_EQ(Photo::AllFields.size(), reflection::enum_values<Photo::Field>.size());
    for (const Photo::Field field : reflection::enum_values<Photo::Field>)
        EXPECT_TRUE(Photo::AllFields.contains(field));
}
