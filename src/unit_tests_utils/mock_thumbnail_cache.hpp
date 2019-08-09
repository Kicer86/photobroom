
#include <gmock/gmock.h>

#include <core/thumbnail_manager.hpp>

struct MockThumbnailCache: IThumbnailCache
{
    MOCK_METHOD2(find, std::optional<QImage>(const QString &, int));
    MOCK_METHOD3(store, void(const QString &, int, const QImage &));
};
