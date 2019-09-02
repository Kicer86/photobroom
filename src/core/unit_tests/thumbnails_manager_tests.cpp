
#include <gmock/gmock.h>

#include <QImage>

#include "unit_tests_utils/fake_task_executor.hpp"
#include "unit_tests_utils/mock_thumbnails_generator.hpp"
#include "unit_tests_utils/mock_thumbnails_cache.hpp"
#include "thumbnail_manager.hpp"


using testing::_;
using testing::Return;

struct MockResponse
{
    MOCK_METHOD1(result, void(QImage));

    void operator()(const QImage& img) { result(img); }
};


TEST(ThumbnailManagerTest, constructs)
{
    EXPECT_NO_THROW(
    {
        FakeTaskExecutor executor;
        ThumbnailManager(&executor, nullptr);
    });
}


TEST(ThumbnailManagerTest, askGeneratorForThumbnailWhenNotCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator);
    tm.fetch(path, height, [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, updateCacheAfterPhotoGeneration)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(path, height, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator, &cache);
    tm.fetch(path, height, [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, doNotGenerateThumbnailFoundInCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(img));

    MockThumbnailsGenerator generator;

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator, &cache);
    tm.fetch(path, height, [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, useGeneratorWhenCacheSetButHasNoResults)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(QImage()));
    EXPECT_CALL(cache, store(path, height, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator, &cache);
    tm.fetch(path, height, [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, returnImageImmediatelyWhenInCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockThumbnailsGenerator generator;
    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator, &cache);
    const std::optional fetchedImg = tm.fetch(path, height);

    ASSERT_TRUE(fetchedImg.has_value());
    EXPECT_EQ(fetchedImg.value(), img);
}


TEST(ThumbnailManagerTest, returnEmptyResultWhenNotInCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(path, height, img)).Times(0);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(0);

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator, &cache);
    const std::optional fetchedImg = tm.fetch(path, height);

    EXPECT_FALSE(fetchedImg.has_value());
}


TEST(ThumbnailManagerTest, cacheThumbnailUnderRequestedHeight)
{
    const QString path = "/some/example/path";
    const int requested_height = 100;
    QImage img;                         // emulate broken image with no height

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, requested_height)).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(path, requested_height, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, requested_height)).Times(1).WillOnce(Return(img));

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, &generator, &cache);
    tm.fetch(path, requested_height, [&response](const QImage& _img){response(_img);});
}
