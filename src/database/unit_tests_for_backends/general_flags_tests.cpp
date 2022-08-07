
#include "common.hpp"

template<typename T>
struct GeneralFlagsTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(GeneralFlagsTest, BackendTypes);


TYPED_TEST(GeneralFlagsTest, flagsIntroduction)
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

    this->m_backend->set(ids[0], "test1", 1);
    this->m_backend->set(ids[0], "test2", 2);
    this->m_backend->set(ids[1], "test3", 3);
    this->m_backend->set(ids[1], "test4", 4);

    EXPECT_EQ(this->m_backend->get(ids[0], "test2"), 2);
    EXPECT_EQ(this->m_backend->get(ids[0], "test1"), 1);
    EXPECT_EQ(this->m_backend->get(ids[1], "test4"), 4);
    EXPECT_EQ(this->m_backend->get(ids[1], "test3"), 3);
}


TYPED_TEST(GeneralFlagsTest, invalidName)
{
    // store 2 photos
    Photo::DataDelta pd1;
    pd1.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::Id> ids;
    std::vector<Photo::DataDelta> photos = { pd1 };
    this->m_backend->addPhotos(photos);

    ids.push_back(photos.front().getId());

    EXPECT_FALSE(this->m_backend->get(ids[0], "test2").has_value());
    EXPECT_FALSE(this->m_backend->get(ids[0], "test1").has_value());
}


TYPED_TEST(GeneralFlagsTest, setAndClearBits)
{
    // store photo
    Photo::DataDelta pd;
    pd.insert<Photo::Field::Path>("photo1.jpeg");

    std::vector<Photo::DataDelta> photos = { pd };
    this->m_backend->addPhotos(photos);

    const auto id = photos.front().getId();

    // perform some bit logic operations
    this->m_backend->set(id, "test1", 0x1);
    EXPECT_EQ(this->m_backend->get(id, "test1"), 0x1);

    this->m_backend->setBits(id, "test1", 0x12);
    EXPECT_EQ(this->m_backend->get(id, "test1"), 0x13);

    this->m_backend->clearBits(id, "test1", 0x2);
    EXPECT_EQ(this->m_backend->get(id, "test1"), 0x11);
}
