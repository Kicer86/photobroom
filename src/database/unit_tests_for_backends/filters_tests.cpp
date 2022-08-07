
#include "common.hpp"

using testing::UnorderedElementsAreArray;


template<typename T>
struct FiltersTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(FiltersTest, BackendTypes);


TYPED_TEST(FiltersTest, generalFlagsFilterTests)
{
    // store 2 photos
    Photo::DataDelta pd1, pd2;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");
    pd2.insert<Photo::Field::Path>("photo2.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1, pd2 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());
    ids.push_back(photos.back().getId());

    this->m_backend->set(ids[0], "test1", 0xfe);
    this->m_backend->set(ids[0], "test2", 0x7f);
    this->m_backend->set(ids[1], "test1", 0xaa);
    this->m_backend->set(ids[1], "test2", 0x55);

    auto flagsFilter1 = Database::FilterPhotosWithGeneralFlag("test1", 5, Database::FilterPhotosWithGeneralFlag::Mode::Bit);
    auto value5Photos = this->m_backend->photoOperator().getPhotos({flagsFilter1});
    EXPECT_TRUE(value5Photos.empty());

    auto flagsFilter2 = Database::FilterPhotosWithGeneralFlag("test1", 2, Database::FilterPhotosWithGeneralFlag::Mode::Bit);
    auto value1Photos = this->m_backend->photoOperator().getPhotos({flagsFilter2});
    EXPECT_THAT(value1Photos, UnorderedElementsAreArray(ids));
}
