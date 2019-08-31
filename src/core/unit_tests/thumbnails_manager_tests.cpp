
#include <gmock/gmock.h>

#include <QImage>

#include "unit_tests_utils/mock_thumbnails_generator.hpp"
#include "unit_tests_utils/mock_thumbnails_cache.hpp"
#include "thumbnail_manager.hpp"


using testing::_;
using testing::Return;

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

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    ThumbnailManager tm(&generator);
    tm.fetch(path, height, [&response](int _h, const QImage& _img){response(_h, _img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, updateCacheAfterPhotoGeneration)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(height, img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(path, height, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    ThumbnailManager tm(&generator);
    tm.setCache(&cache);
    tm.fetch(path, height, [&response](int _h, const QImage& _img){response(_h, _img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, doNotGenerateThumbnailFoundInCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(height, img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(img));

    MockThumbnailsGenerator generator;

    ThumbnailManager tm(&generator);
    tm.setCache(&cache);
    tm.fetch(path, height, [&response](int _h, const QImage& _img){response(_h, _img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, useGeneratorWhenCacheSetButHasNoResults)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(height, img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(QImage()));
    EXPECT_CALL(cache, store(path, height, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    ThumbnailManager tm(&generator);
    tm.setCache(&cache);
    tm.fetch(path, height, [&response](int _h, const QImage& _img){response(_h, _img);});  // mock cannot be used here directly
}


TEST(ThumbnailManagerTest, returnImageImmediatelyWhenInCache)
{
    const QString path = "/some/example/path";
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockThumbnailsGenerator generator;
    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(path, height)).Times(1).WillOnce(Return(img));

    ThumbnailManager tm(&generator, &cache);
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
    EXPECT_CALL(cache, store(path, height, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(path, height)).Times(1).WillOnce(Return(img));

    ThumbnailManager tm(&generator, &cache);
    const std::optional fetchedImg = tm.fetch(path, height);

    EXPECT_FALSE(fetchedImg.has_value());
}
