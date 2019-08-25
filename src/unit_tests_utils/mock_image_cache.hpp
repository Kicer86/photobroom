
#include <gmock/gmock.h>

#include <core/iimage_cache.hpp>

struct MockImageCache: IImageCache
{
    MOCK_METHOD2(find, std::optional<QImage>(const QString &, int));
    MOCK_METHOD3(store, void(const QString &, int, const QImage &));
};
