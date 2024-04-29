
#include <magic_enum.hpp>

#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/rich_db.json.hpp"
#include "unit_tests_utils/sample_db.json.hpp"

#include "common.hpp"


template<typename T>
struct PhotosTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(PhotosTest, BackendTypes);


TYPED_TEST(PhotosTest, insertion)
{
    std::vector<Photo::Id> reported_ids;

    QObject::connect(this->m_backend.get(), &Database::IBackend::photosAdded, [&reported_ids](const std::vector<Photo::Id>& ids)
    {
        reported_ids.insert(reported_ids.end(), ids.begin(), ids.end());
    });

    Database::JsonToBackend converter(*this->m_backend.get());
    converter.append(SampleDB::db1);

    EXPECT_EQ(reported_ids.size(), 3);
}


TYPED_TEST(PhotosTest, retrievingAllDataInDelta)
{
    std::vector<Photo::Id> reported_ids;

    Database::JsonToBackend converter(*this->m_backend.get());
    converter.append(RichDB::db1);

    const auto ids = this->m_backend->photoOperator().getPhotos(Database::EmptyFilter());
    ASSERT_EQ(ids.size(), 3);

    for (const auto& id: ids)
    {
        const auto photoDelta = this->m_backend->getPhotoDelta(id);

        EXPECT_TRUE(photoDelta.has(Photo::Field::Path));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Tags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Flags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Geometry));
        EXPECT_TRUE(photoDelta.has(Photo::Field::PHash));
    }
}


TYPED_TEST(PhotosTest, retrievingPartialDataInDelta)
{
    std::vector<Photo::Id> reported_ids;

    Database::JsonToBackend converter(*this->m_backend.get());
    converter.append(RichDB::db1);

    const auto ids = this->m_backend->photoOperator().getPhotos(Database::EmptyFilter());
    ASSERT_EQ(ids.size(), 3);

    for (const auto& id: ids)
    {
        const auto photoDelta = this->m_backend->getPhotoDelta(id, {Photo::Field::Path, Photo::Field::Flags});

        EXPECT_TRUE(photoDelta.has(Photo::Field::Path));
        EXPECT_FALSE(photoDelta.has(Photo::Field::Tags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Flags));
        EXPECT_FALSE(photoDelta.has(Photo::Field::Geometry));
    }

    for (const auto& id: ids)
    {
        auto photoDelta = this->m_backend->getPhotoDelta(id, {Photo::Field::Tags, Photo::Field::Geometry});

        EXPECT_FALSE(photoDelta.has(Photo::Field::Path));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Tags));
        EXPECT_FALSE(photoDelta.has(Photo::Field::Flags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Geometry));
    }
}
