
#include <gmock/gmock.h>

#include "gui/desktop/utils/thumbnails_cache.hpp"


TEST(ThumbnailsCacheTest, isConstructible)
{
    ThumbnailsCache cache;
}

TEST(ThumbnailsCacheTest, returnsNothingWhenEmpty)
{
    ThumbnailsCache cache;

    std::optional img1 = cache.find(Photo::Id(1), QSize(0, 0));
    std::optional img2 = cache.find(Photo::Id(100), QSize(100, 100));
    std::optional img3 = cache.find(Photo::Id(3456), QSize(-100, -100));

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
    cache.store(Photo::Id(1), QSize(height1, height1), img1);
    cache.store(Photo::Id(2), QSize(height2, height2), img2);
    cache.store(Photo::Id(3), QSize(height3, height3), img3);

    const std::optional img1a = cache.find(Photo::Id(1), QSize(0, 0));
    const std::optional img1b = cache.find(Photo::Id(1), QSize(height1, height1));
    const std::optional img1c = cache.find(Photo::Id(1), QSize(height1 * 2, height1 * 2));

    const std::optional img2a = cache.find(Photo::Id(2), QSize(0, 0));
    const std::optional img2b = cache.find(Photo::Id(2), QSize(height2, height2));
    const std::optional img2c = cache.find(Photo::Id(2), QSize(height2 * 2, height2 * 2));

    const std::optional img3a = cache.find(Photo::Id(3), QSize(0, 0));
    const std::optional img3b = cache.find(Photo::Id(3), QSize(height3, height3));
    const std::optional img3c = cache.find(Photo::Id(3), QSize(height3 * 2, height3 * 2));

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
