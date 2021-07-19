
#include <gmock/gmock.h>

#include <QImage>

#include <core/constants.hpp>
#include "unit_tests_utils/fake_task_executor.hpp"
#include "unit_tests_utils/mock_thumbnails_generator.hpp"
#include "unit_tests_utils/mock_thumbnails_cache.hpp"
#include "unit_tests_utils/mock_backend.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/printers.hpp"
#include "utils/thumbnail_manager.hpp"


using namespace std::placeholders;
using testing::_;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::ReturnRef;

struct NullCache: IThumbnailsCache
{
    std::optional<QImage> find(const Photo::Id &, const IThumbnailsCache::ThumbnailParameters &) override
    {
        return {};
    }

    void store(const Photo::Id &, const IThumbnailsCache::ThumbnailParameters &, const QImage &) override
    {

    }
};

struct MockResponse
{
    MOCK_METHOD1(result, void(QImage));

    void operator()(const QImage& img) { result(img); }
};


class ThumbnailManagerTest: public testing::Test
{
public:
    ThumbnailManagerTest()
    {
        ON_CALL(db, execute(_)).WillByDefault(Invoke([this](std::unique_ptr<Database::IDatabase::ITask>&& task)
        {
            task->run(backend);
        }));

        ON_CALL(db, backend).WillByDefault(ReturnRef(backend));

        ON_CALL(backend, getPhotoDelta).WillByDefault(Invoke([](const auto& id, const auto &)
        {
            Photo::DataDelta delta(id);
            delta.insert<Photo::Field::Path>(QString("%1.jpeg").arg(id.value()));

            return delta;
        }));
    }

    NiceMock<MockDatabase> db;
    NiceMock<MockBackend> backend;
};


TEST_F(ThumbnailManagerTest, constructs)
{
    EXPECT_NO_THROW(
    {
        FakeTaskExecutor executor;
        MockThumbnailsGenerator generator;
        MockThumbnailsCache cache;
        ThumbnailManager(&executor, generator, cache);
    });
}


TEST_F(ThumbnailManagerTest, askGeneratorForThumbnailWhenNoCache)
{
    const Photo::Id id(5);
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);
    QImage base_img(Parameters::databaseThumbnailSize, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(_, IThumbnailsCache::ThumbnailParameters(Parameters::databaseThumbnailSize))).Times(1).WillOnce(Return(base_img));
    EXPECT_CALL(generator, generateFrom(base_img, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).WillRepeatedly(Return(img));

    FakeTaskExecutor executor;

    NullCache cache;
    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    tm.fetch(id, QSize(height, height), [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST_F(ThumbnailManagerTest, updateCacheAfterPhotoGeneration)
{
    const Photo::Id id = Photo::Id(7);
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);
    QImage base_img(Parameters::databaseThumbnailSize, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)), img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(_, IThumbnailsCache::ThumbnailParameters(Parameters::databaseThumbnailSize))).Times(1).WillOnce(Return(base_img));
    EXPECT_CALL(generator, generateFrom(base_img, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).WillRepeatedly(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    tm.fetch(id, QSize(height, height), [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST_F(ThumbnailManagerTest, doNotGenerateThumbnailFoundInCache)
{
    const Photo::Id id(11);
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).Times(1).WillOnce(Return(img));

    MockThumbnailsGenerator generator;

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    tm.fetch(id, QSize(height, height), [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST_F(ThumbnailManagerTest, useGeneratorWhenCacheSetButHasNoResults)
{
    const Photo::Id id(13);
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);
    QImage base_img(Parameters::databaseThumbnailSize, QImage::Format_RGB32);

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).Times(1).WillOnce(Return(QImage()));
    EXPECT_CALL(cache, store(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)), img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(_, IThumbnailsCache::ThumbnailParameters(Parameters::databaseThumbnailSize))).Times(1).WillOnce(Return(base_img));
    EXPECT_CALL(generator, generateFrom(base_img, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).WillRepeatedly(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    tm.fetch(id, QSize(height, height), [&response](const QImage& _img){response(_img);});  // mock cannot be used here directly
}


TEST_F(ThumbnailManagerTest, returnImageImmediatelyWhenInCache)
{
    const Photo::Id id(17);
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockThumbnailsGenerator generator;
    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).Times(1).WillOnce(Return(img));

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    const std::optional fetchedImg = tm.fetch(id, QSize(height, height));

    ASSERT_TRUE(fetchedImg.has_value());
    EXPECT_EQ(fetchedImg.value(), img);
}


TEST_F(ThumbnailManagerTest, returnEmptyResultWhenNotInCache)
{
    const Photo::Id id(19);
    const int height = 100;
    QImage img(height * 2, height, QImage::Format_RGB32);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(id, IThumbnailsCache::ThumbnailParameters(QSize(height, height)), img)).Times(0);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(_, IThumbnailsCache::ThumbnailParameters(QSize(height, height)))).Times(0);

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    const std::optional fetchedImg = tm.fetch(id, QSize(height, height));

    EXPECT_FALSE(fetchedImg.has_value());
}


TEST_F(ThumbnailManagerTest, cacheThumbnailUnderRequestedHeight)
{
    const Photo::Id id(23);
    const int requested_height = 100;
    QImage img;                         // emulate broken image with no height
    QImage base_img(Parameters::databaseThumbnailSize, QImage::Format_RGB32);

    MockThumbnailsCache cache;
    EXPECT_CALL(cache, find(id, { QSize(requested_height, requested_height) })).Times(1).WillOnce(Return(std::optional<QImage>{}));
    EXPECT_CALL(cache, store(id, { QSize(requested_height, requested_height) }, img)).Times(1);

    MockThumbnailsGenerator generator;
    EXPECT_CALL(generator, generate(_, IThumbnailsCache::ThumbnailParameters(Parameters::databaseThumbnailSize))).Times(1).WillOnce(Return(base_img));
    EXPECT_CALL(generator, generateFrom(base_img, IThumbnailsCache::ThumbnailParameters(QSize(requested_height, requested_height)))).WillRepeatedly(Return(img));

    MockResponse response;
    EXPECT_CALL(response, result(img)).Times(1);

    FakeTaskExecutor executor;

    ThumbnailManager tm(&executor, generator, cache);
    tm.setDatabaseCache(&db);
    tm.fetch(id, QSize(requested_height, requested_height), [&response](const QImage& _img){response(_img);});
}
