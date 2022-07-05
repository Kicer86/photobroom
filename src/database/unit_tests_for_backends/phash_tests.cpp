
#include "database/filter.hpp"
#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "unit_tests_utils/phash_db.json.hpp"

#include "common.hpp"

using testing::Contains;
using testing::UnorderedElementsAre;


template<typename T>
struct PHashTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(PHashTest, BackendTypes);


TYPED_TEST(PHashTest, noPHashByDefault)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto photos = this->m_backend->photoOperator().getPhotos({});

    ASSERT_EQ(photos.size(), 3);

    for(const auto& id: photos)
    {
        const bool has = this->m_backend->photoOperator().hasPHash(id);
        EXPECT_FALSE(has);

        const auto value = this->m_backend->photoOperator().getPHash(id);
        EXPECT_FALSE(value.has_value());
    }
}


TYPED_TEST(PHashTest, readWhatWasWritten)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto photos = this->m_backend->photoOperator().getPhotos({});

    for(const auto& id: photos)
    {
        const Photo::PHash phash(id.value());
        this->m_backend->photoOperator().setPHash(id, phash);

        ASSERT_TRUE(this->m_backend->photoOperator().hasPHash(id));

        const auto phash_read = this->m_backend->photoOperator().getPHash(id);
        ASSERT_TRUE(phash_read.has_value());

        EXPECT_EQ(phash, *phash_read);
    }
}


TYPED_TEST(PHashTest, replacePHash)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto photos = this->m_backend->photoOperator().getPhotos({});
    ASSERT_FALSE(photos.empty());

    const auto id = photos.front();
    const Photo::PHash phash1(0x1122334455667788LL);
    const Photo::PHash phash2(0x7900aabbccddeeffLL);

    this->m_backend->photoOperator().setPHash(id, phash1);
    this->m_backend->photoOperator().setPHash(id, phash2);

    const auto phash_read = this->m_backend->photoOperator().getPHash(id);
    ASSERT_TRUE(phash_read.has_value());

    EXPECT_EQ(phash2, *phash_read);
}


TYPED_TEST(PHashTest, findSimilar)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(PHashDB::db);
    converter.append(SampleDB::db1);            // append photos without hash

    // find duplicated
    Database::FilterSimilarPhotos filter;
    const auto photos = this->m_backend->photoOperator().getPhotos(filter);

    EXPECT_EQ(photos.size(), 7);
}


TYPED_TEST(PHashTest, filterPhotosWithPHash)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);            // append photos without hash

    //
    const auto photos = this->m_backend->photoOperator().getPhotos({});
    ASSERT_EQ(photos.size(), 3);

    const Photo::PHash phash1(0x1122334455667788LL);
    const Photo::PHash phash2;

    this->m_backend->photoOperator().setPHash(photos[0], phash1);
    this->m_backend->photoOperator().setPHash(photos[1], phash2);

    // filter photos
    Database::FilterPhotosWithPHash withPhash;
    const auto noPhashPhotos = this->m_backend->photoOperator().getPhotos(withPhash);

    EXPECT_THAT(noPhashPhotos, UnorderedElementsAre(photos[0]));
}
