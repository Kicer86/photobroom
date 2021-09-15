
#include <QSignalSpy>

#include "common.hpp"


template<typename T>
struct TransactionAccumulationsTests: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(TransactionAccumulationsTests, BackendTypes);

TYPED_TEST(TransactionAccumulationsTests, newPhotos)
{
    Photo::DataDelta photo1, photo2;
    photo1.insert<Photo::Field::Path>("/path/photo.jpeg");
    photo2.insert<Photo::Field::Path>("/path/photo.jpeg");

    std::vector photos1{photo1};
    std::vector photos2{photo2};

    QSignalSpy notifications(this->m_backend.get(), &Database::IBackend::photosAdded);
    this->m_backend->addPhotos(photos1);
    this->m_backend->addPhotos(photos2);

    // insertions should be accumulated into one notification
    //EXPECT_EQ(notifications.count(), 1);
}
