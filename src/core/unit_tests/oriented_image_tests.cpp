
#include <gmock/gmock.h>

#include <QImage>

#include <unit_tests_utils/mock_exif_reader.hpp>
#include "oriented_image.hpp"


TEST(OrientedImageTest, constructor)
{
    // TODO: build issues...
    //       probably broken by https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90415

    /*
    OrientedImage();

    QImage qimage(10, 100, QImage::Format_RGB32);
    MockExifReader exif;

    OrientedImage(&exif, qimage);
    */
}
