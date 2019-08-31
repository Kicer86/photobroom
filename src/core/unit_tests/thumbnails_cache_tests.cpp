
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
