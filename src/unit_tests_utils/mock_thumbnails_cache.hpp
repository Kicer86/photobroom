
#include <gmock/gmock.h>

#include <core/ithumbnails_cache.hpp>

struct MockThumbnailsCache: IThumbnailsCache
{
    MOCK_METHOD(std::optional<QImage>, find, (const QString &, const IThumbnailsCache::ThumbnailParameters& params), (override));
    MOCK_METHOD(void, store, (const QString &, const IThumbnailsCache::ThumbnailParameters& params, const QImage &), (override));
};
