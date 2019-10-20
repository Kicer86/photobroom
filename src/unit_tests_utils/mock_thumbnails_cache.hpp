
#include <gmock/gmock.h>

#include <core/ithumbnails_cache.hpp>

struct MockThumbnailsCache: IThumbnailsCache
{
    MOCK_METHOD(std::optional<QImage>, find, (const QString &, int), (override));
    MOCK_METHOD(void, store, (const QString &, int, const QImage &), (override));
};
