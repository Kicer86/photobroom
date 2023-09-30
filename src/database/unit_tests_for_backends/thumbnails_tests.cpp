
#include "common.hpp"

namespace
{
    const QString ThumbnailBlob = "thumbnail";
}

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

    this->m_backend->writeBlob(id, ThumbnailBlob, QByteArray("thumbnail"));
    const QByteArray thumbnail = this->m_backend->readBlob(id, ThumbnailBlob);

    EXPECT_EQ(thumbnail, "thumbnail");
}


TYPED_TEST(ThumbnailsTest, thumbnailOverride)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();

    this->m_backend->writeBlob(id, ThumbnailBlob, QByteArray("thumbnail"));
    this->m_backend->writeBlob(id, ThumbnailBlob, QByteArray("thumbnail2"));
    const QByteArray thumbnail = this->m_backend->readBlob(id, ThumbnailBlob);

    EXPECT_EQ(thumbnail, "thumbnail2");
}


TYPED_TEST(ThumbnailsTest, emptyResultWhenMissing)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();
    const QByteArray thumbnail = this->m_backend->readBlob(id, ThumbnailBlob);

    EXPECT_TRUE(thumbnail.isEmpty());
}
