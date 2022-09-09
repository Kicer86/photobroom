
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


TEST(WebPGeneratorTest, multiFrameImage)
{
    QImage frame1(100, 100, QImage::Format_ARGB32);
    frame1.fill(Qt::red);

    QImage frame2(100, 100, QImage::Format_ARGB32);
    frame2.fill(Qt::blue);

    QImage frame3(100, 100, QImage::Format_ARGB32);
    frame3.fill(Qt::green);

    auto data = WebPGenerator().append(frame1).append(frame2).append(frame3).setLossless().save();

    QBuffer buffer(&data);
    QImageReader reader(&buffer);

    ASSERT_EQ(reader.imageCount(), 3);

    const auto image1 = reader.read();
    EXPECT_EQ(image1.pixelColor(50, 50), frame1.pixelColor(50, 50));

    const auto image2 = reader.read();
    EXPECT_EQ(image2.pixelColor(50, 50), frame2.pixelColor(50, 50));

    const auto image3 = reader.read();
    EXPECT_EQ(image3.pixelColor(50, 50), frame3.pixelColor(50, 50));
}


TEST(WebPGeneratorTest, imagesDelay)
{
    using namespace std::chrono_literals;

    QImage frame1(100, 100, QImage::Format_ARGB32);
    frame1.fill(Qt::red);

    QImage frame2(100, 100, QImage::Format_ARGB32);
    frame2.fill(Qt::blue);

    auto data = WebPGenerator().append(frame1).append(frame2).setDelay(300ms).save();

    QBuffer buffer(&data);
    QImageReader reader(&buffer);

    ASSERT_EQ(reader.imageCount(), 2);

    reader.read();
    EXPECT_EQ(reader.nextImageDelay(), 300);

    reader.read();
    EXPECT_EQ(reader.nextImageDelay(), 300);
}


TEST(WebPGeneratorTest, loopDelay)
{
    using namespace std::chrono_literals;

    QImage frame1(100, 100, QImage::Format_ARGB32);
    frame1.fill(Qt::red);

    QImage frame2(100, 100, QImage::Format_ARGB32);
    frame2.fill(Qt::blue);

    QImage frame3(100, 100, QImage::Format_ARGB32);
    frame3.fill(Qt::green);

    auto data = WebPGenerator().append(frame1).append(frame2).append(frame3).setLoopDelay(500ms).save();

    QBuffer buffer(&data);
    QImageReader reader(&buffer);

    ASSERT_EQ(reader.imageCount(), 3);

    reader.read();
    reader.read();
    reader.read();
    EXPECT_EQ(reader.nextImageDelay(), 500 + 100);  // last frame's duration + delay before loop is being reset
}
