
#include <gmock/gmock.h>

#include <QImage>

#include "unit_tests_utils/mock_athumbnail_generator.hpp"
#include "unit_tests_utils/mock_thumbnail_cache.hpp"
#include "thumbnail_manager.hpp"


using testing::_;

struct MockResponse
{
    MOCK_METHOD2(result, void(int, QImage));

    void operator()(int h, const QImage& img) { result(h, img); }
};


TEST(ThumbnailManagerTest, constructs)
{
    EXPECT_NO_THROW(
    {
        ThumbnailManager(nullptr);
    });
}


TEST(ThumbnailManagerTest, askGeneratorForThumbnailWhenNoCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(height, img)).Times(1);

    MockThumbnailGenerator generator;
    EXPECT_CALL(generator, run(path, height, _)).Times(1).WillOnce([&img](const QString &, int, std::unique_ptr<MockThumbnailGenerator::ICallback> callback)
    {
        callback->result(img);
    });

    ThumbnailManager tm(&generator);
    tm.fetch(path, height, response);
}


TEST(ThumbnailManagerTest, updateCacheAfterPhotoGeneration)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(height, img)).Times(1);

    MockThumbnailCache cache;
    EXPECT_CALL(cache, store(path, height, img)).Times(1);

    MockThumbnailGenerator generator;
    EXPECT_CALL(generator, run(path, height, _)).Times(1).WillOnce([&img](const QString &, int, std::unique_ptr<MockThumbnailGenerator::ICallback> callback)
    {
        callback->result(img);
    });

    ThumbnailManager tm(&generator);
    tm.setCache(&cache);
    tm.fetch(path, height, response);
}
