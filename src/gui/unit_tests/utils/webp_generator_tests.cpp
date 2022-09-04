
#include <gtest/gtest.h>
#include <QBuffer>
#include <QImageReader>

#include "utils/webp_generator.hpp"

TEST(WebPGeneratorTest, singleFrameImage)
{
    QImage frame(100, 100, QImage::Format_ARGB32);
    frame.fill(Qt::red);

    auto data = WebPGenerator().append(frame).setLossless().save();

    QBuffer buffer(&data);
    QImageReader reader(&buffer);
    const auto image = reader.read();

    EXPECT_EQ(image.pixelColor(50, 50), frame.pixelColor(50, 50));
}
