
#include <gmock/gmock.h>

#include <core/thumbnails_cache.hpp>

TEST(ThumbnailsCacheTest, isConstructible)
{
    ThumbnailsCache cache;
}

TEST(ThumbnailsCacheTest, returnsNothingWhenEmpty)
{
    ThumbnailsCache cache;

    std::optional img1 = cache.find("", 0);
    std::optional img2 = cache.find("/", 100);
    std::optional img3 = cache.find(".", -100);

    EXPECT_FALSE(img1.has_value());
    EXPECT_FALSE(img2.has_value());
    EXPECT_FALSE(img3.has_value());
}


TEST(ThumbnailsCacheTest, returnsWhatWasStored)
{
    const int height1 = 100;
    const QImage img1(height1 * 2, height1, QImage::Format_RGB32);

    const int height2 = 200;
    const QImage img2(height2 * 2, height2, QImage::Format_RGB32);

    const int height3 = 300;
    const QImage img3(height3 * 2, height3, QImage::Format_RGB32);

    ThumbnailsCache cache;
    cache.store("img1", height1, img1);
    cache.store("img2", height2, img2);
    cache.store("img3", height3, img3);

    const std::optional img1a = cache.find("img1", 0);
    const std::optional img1b = cache.find("img1", height1);
    const std::optional img1c = cache.find("img1", height1 * 2);

    const std::optional img2a = cache.find("img2", 0);
    const std::optional img2b = cache.find("img2", height2);
    const std::optional img2c = cache.find("img2", height2 * 2);

    const std::optional img3a = cache.find("img3", 0);
    const std::optional img3b = cache.find("img3", height3);
    const std::optional img3c = cache.find("img3", height3 * 2);

    EXPECT_FALSE(img1a.has_value());
    EXPECT_TRUE(img1b.has_value());
    EXPECT_FALSE(img1c.has_value());

    EXPECT_FALSE(img2a.has_value());
    EXPECT_TRUE(img2b.has_value());
    EXPECT_FALSE(img2c.has_value());

    EXPECT_FALSE(img3a.has_value());
    EXPECT_TRUE(img3b.has_value());
    EXPECT_FALSE(img3c.has_value());
}

