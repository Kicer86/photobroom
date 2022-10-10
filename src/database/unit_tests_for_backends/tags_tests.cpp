
#include <QDate>

#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/sample_db2.json.hpp"

#include "common.hpp"

using testing::Contains;


template<typename T>
struct TagsTest: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(TagsTest, BackendTypes);


TYPED_TEST(TagsTest, listTagValues)
{
    Database::JsonToBackend converter(*this->m_backend.get());
    converter.append(SampleDB::db2);

    const auto all_dates = this->m_backend->listTagValues(Tag::Types::Date, {});

    ASSERT_EQ(all_dates.size(), 7);
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.01", Qt::ISODate))));
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.02", Qt::ISODate))));
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.03", Qt::ISODate))));
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.04", Qt::ISODate))));
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.05", Qt::ISODate))));
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.06", Qt::ISODate))));
    EXPECT_THAT(all_dates, Contains(TagValue(QDate::fromString("2001.01.07", Qt::ISODate))));
}


TYPED_TEST(TagsTest, removeTagValue)
{
    Database::JsonToBackend converter(*this->m_backend.get());
    converter.append(SampleDB::db2);

    const auto photos = this->m_backend->photoOperator().getPhotos({});
    ASSERT_FALSE(photos.empty());

    const auto& photoId = photos.front();

    // remove tag
    {
        Photo::DataDelta photo = this->m_backend->getPhotoDelta(photoId, {Photo::Field::Tags});

        auto& tags = photo.get<Photo::Field::Tags>();
        ASSERT_FALSE(tags.empty());

        auto dateIt = tags.find(Tag::Types::Date);
        ASSERT_NE(dateIt, tags.end());

        tags.erase(dateIt);

        this->m_backend->update({photo});
    }

    // refetch photo to verify if tag was removed
    {
        Photo::DataDelta photo = this->m_backend->getPhotoDelta(photoId, {Photo::Field::Tags});
        auto& tags = photo.get<Photo::Field::Tags>();

        EXPECT_EQ(tags.find(Tag::Types::Date), tags.end());         // Date should be gone
    }
}
