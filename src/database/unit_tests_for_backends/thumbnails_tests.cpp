
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

    this->m_backend->setThumbnail(id, QByteArray("thumbnail"));
    const QByteArray thumbnail = this->m_backend->getThumbnail(id);

    EXPECT_EQ(thumbnail, "thumbnail");
}


TYPED_TEST(ThumbnailsTest, thumbnailOverride)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();

    this->m_backend->setThumbnail(id, QByteArray("thumbnail"));
    this->m_backend->setThumbnail(id, QByteArray("thumbnail2"));
    const QByteArray thumbnail = this->m_backend->getThumbnail(id);

    EXPECT_EQ(thumbnail, "thumbnail2");
}


TYPED_TEST(ThumbnailsTest, emptyResultWhenMissing)
{
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/path/photo.jpeg");
    std::vector<Photo::DataDelta> photos { photo };

    this->m_backend->addPhotos(photos);
    const auto id = photos.front().getId();
    const QByteArray thumbnail = this->m_backend->getThumbnail(id);

    EXPECT_TRUE(thumbnail.isEmpty());
}
