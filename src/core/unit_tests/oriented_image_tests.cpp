
#include <gmock/gmock.h>

#include <QImage>

#include "../implementation/exiv2_exif_reader.hpp"
#include "../oriented_image.hpp"


class OrientedImageTest: public testing::TestWithParam<QLatin1StringView> {};

INSTANTIATE_TEST_SUITE_P(RotatedImages,
                         OrientedImageTest,
                         testing::Values(
                             "f/f1-exif.jpg",
                             "f/f2-exif.jpg",
                             "f/f3-exif.jpg",
                             "f/f4-exif.jpg",
                             "f/f5-exif.jpg",
                             "f/f6-exif.jpg",
                             "f/f7-exif.jpg",
                             "f/f8-exif.jpg"
                        )
);


TEST_P(OrientedImageTest, allOrientations)
{
    const auto fileName = GetParam();

    Exiv2ExifReader reader;
    OrientedImage orientedImage(reader, QString(IMAGES_DIR) + "/" + QString(fileName));

    const QImage referenceImage(QString(IMAGES_DIR) + "/f/f1.jpg");

    EXPECT_EQ(referenceImage, orientedImage.get());
}
