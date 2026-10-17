
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


TYPED_TEST(PhotosTest, properlyStoriesComplexPaths)
{
    Filesystem::Location l1("/11/22/345.qwe"),
                         l2(QStringList{"/12/34/56.qwerty", "/sddg/gretwq/dsd"}),
                         l3(QStringList{"/12/34/56", "/sddg/gretwq/dsd/", "ds21/111.ert"}),
                         l4(QStringList{"/12sdfdsfd/34/", "/sddg/gretwq/dsd/", "/ds21/111.ert"});

    Photo::DataDelta p1(l1),
                     p2(l2),
                     p3(l3),
                     p4(l4);

    std::vector photos = {p1, p2, p3, p4};
    const bool status = this->m_backend->addPhotos(photos);
    ASSERT_TRUE(status);

    const auto p1_asRead = this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[0].getId());
    const auto p2_asRead = this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[1].getId());
    const auto p3_asRead = this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[2].getId());
    const auto p4_asRead = this->m_backend->template getPhotoDelta<Photo::Field::Path>(photos[3].getId());

    EXPECT_EQ(p1_asRead.template get<Photo::Field::Path>(), l1);
    EXPECT_EQ(p2_asRead.template get<Photo::Field::Path>(), l2);
    EXPECT_EQ(p3_asRead.template get<Photo::Field::Path>(), l3);
    EXPECT_EQ(p4_asRead.template get<Photo::Field::Path>(), l4);
}
