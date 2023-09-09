
#include "common.hpp"


template<typename T>
struct ThumbnailsTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(ThumbnailsTest, BackendTypes);


TYPED_TEST(ThumbnailsTest, storesThumbnail)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();

    this->m_backend->writeBlob(id, Database::IBackend::BlobType::Thumbnail, QByteArray("thumbnail"));
    const QByteArray thumbnail = this->m_backend->readBlob(id, Database::IBackend::BlobType::Thumbnail);

    EXPECT_EQ(thumbnail, "thumbnail");
}


TYPED_TEST(ThumbnailsTest, thumbnailOverride)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();

    this->m_backend->writeBlob(id, Database::IBackend::BlobType::Thumbnail, QByteArray("thumbnail"));
    this->m_backend->writeBlob(id, Database::IBackend::BlobType::Thumbnail, QByteArray("thumbnail2"));
    const QByteArray thumbnail = this->m_backend->readBlob(id, Database::IBackend::BlobType::Thumbnail);

    EXPECT_EQ(thumbnail, "thumbnail2");
}


TYPED_TEST(ThumbnailsTest, emptyResultWhenMissing)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();
    const QByteArray thumbnail = this->m_backend->readBlob(id, Database::IBackend::BlobType::Thumbnail);

    EXPECT_TRUE(thumbnail.isEmpty());
}
