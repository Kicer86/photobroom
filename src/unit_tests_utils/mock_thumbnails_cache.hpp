
#include <gmock/gmock.h>

#include <gui/desktop/utils/ithumbnails_cache.hpp>

struct MockThumbnailsCache: IThumbnailsCache
{
    MOCK_METHOD(std::optional<QImage>, find, (const Photo::Id &, const IThumbnailsCache::ThumbnailParameters& params), (override));
    MOCK_METHOD(void, store, (const Photo::Id &, const IThumbnailsCache::ThumbnailParameters& params, const QImage &), (override));
};
