
#include <gmock/gmock.h>

#include <core/ithumbnails_cache.hpp>

struct MockThumbnailsCache: IThumbnailsCache
{
    MOCK_METHOD2(find, std::optional<QImage>(const QString &, int));
    MOCK_METHOD3(store, void(const QString &, int, const QImage &));
};
