
#include <magic_enum.hpp>

#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/rich_db.json.hpp"
#include "unit_tests_utils/sample_db.json.hpp"

#include "common.hpp"


namespace
{
    bool same(const Photo::Data& data, Photo::DataDelta& delta)
    {
        if (delta.getId() != data.id)
            return false;

        constexpr auto& fields = magic_enum::enum_values<Photo::Field>();
        for (const auto& field: fields)
        {
            switch (field)
            {
                case Photo::Field::Tags:
                    if (delta.has(Photo::Field::Tags) && delta.get<Photo::Field::Tags>() != data.tags)
                        return false;
                    break;

                case Photo::Field::Flags:
                    if (delta.has(Photo::Field::Flags) && delta.get<Photo::Field::Flags>() != data.flags)
                        return false;
                    break;

                case Photo::Field::Path:
                    if (delta.has(Photo::Field::Path) && delta.get<Photo::Field::Path>() != data.path)
                        return false;
                    break;

                case Photo::Field::Geometry:
                    if (delta.has(Photo::Field::Geometry) && delta.get<Photo::Field::Geometry>() != data.geometry)
                        return false;
                    break;

                case Photo::Field::GroupInfo:
                    if (delta.has(Photo::Field::GroupInfo) && delta.get<Photo::Field::GroupInfo>() != data.groupInfo)
                        return false;
                    break;

                case Photo::Field::PHash:
                    if (delta.has(Photo::Field::PHash) && delta.get<Photo::Field::PHash>() != data.phash)
                        return false;
                    break;
            }
        }

        return true;
    }
}


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
        auto photo = this->m_backend->getPhoto(id);
        auto photoDelta = this->m_backend->getPhotoDelta(id);

        EXPECT_TRUE(photoDelta.has(Photo::Field::Path));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Tags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Flags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Geometry));
        EXPECT_TRUE(photoDelta.has(Photo::Field::PHash));

        EXPECT_TRUE(same(photo, photoDelta));
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
        auto photo = this->m_backend->getPhoto(id);
        auto photoDelta = this->m_backend->getPhotoDelta(id, {Photo::Field::Path, Photo::Field::Flags});

        EXPECT_TRUE(photoDelta.has(Photo::Field::Path));
        EXPECT_FALSE(photoDelta.has(Photo::Field::Tags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Flags));
        EXPECT_FALSE(photoDelta.has(Photo::Field::Geometry));

        EXPECT_TRUE(same(photo, photoDelta));
    }

    for (const auto& id: ids)
    {
        auto photo = this->m_backend->getPhoto(id);
        auto photoDelta = this->m_backend->getPhotoDelta(id, {Photo::Field::Tags, Photo::Field::Geometry});

        EXPECT_FALSE(photoDelta.has(Photo::Field::Path));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Tags));
        EXPECT_FALSE(photoDelta.has(Photo::Field::Flags));
        EXPECT_TRUE(photoDelta.has(Photo::Field::Geometry));

        EXPECT_TRUE(same(photo, photoDelta));
    }
}
