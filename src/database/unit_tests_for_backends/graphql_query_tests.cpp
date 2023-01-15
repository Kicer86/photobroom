
#include "database/photo_utils.hpp"
#include "database_tools/json_to_backend.hpp"
#include "database/query_response_parser.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "unit_tests_utils/phash_db.json.hpp"

#include "common.hpp"


using testing::Eq;
using testing::ResultOf;
using testing::UnorderedElementsAre;


namespace
{

inline const char* test_db = R"(
{
    "photos": [
        {
            "path": "/some/path1.jpeg",
            "tags": { "date":  "2001.01.01", "time":  "10:00:00", "event": "Some event", "place": "Internet" }
        },
        {
            "path": "/some/path2.jpeg",
            "tags": { "date":  "2001.01.01", "time":  "10:00:00", "event": "", "place": "Internet" },
            "phash": 1
        },
        {
            "path": "/some/path3.jpeg",
            "tags": { "date":  "2001.01.01", "time":  "11:00:00", "event": "Another event", "place": "" },
            "phash": 2
        },
        {
            "path": "/some/path4.jpeg",
            "tags": { "date":  "2001.01.02", "time":  "11:00:00", "event": "Another event", "place": "" }
        }
    ]
})";

}


template<typename T>
struct QueryTests: DatabaseTest<T>
{
    template<std::size_t count>
    std::array<Tag::TagsList, count> readTags(const QString& response)
    {
        const std::vector<Photo::DataDelta> photos = ResponseParser::parsePhotosQueryResponse(response);
        assert(photos.size() == count);

        std::array<Tag::TagsList, count> tags;
        for(auto i = 0u; i < count; i++)
        {
            const Photo::DataDelta& data = photos[i];
            assert(data.has(Photo::Field::Tags));

            tags[i] = data.get<Photo::Field::Tags>();
        }

        return tags;
    }
};

TYPED_TEST_SUITE(QueryTests, BackendTypes);


TYPED_TEST(QueryTests, getPhotos)
{
    // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto response = this->m_backend->query(
        Query::commonFragments() +
        QStringLiteral(
        R"(
            query {
                photos {
                    id
                    tags{
                        ...TagsFields
                    }
                }
            }
        )"));

    constexpr std::size_t expectedPhotos = 3;
    const auto tags = this-> template readTags<expectedPhotos>(response);

    EXPECT_THAT(tags, UnorderedElementsAre(
        Tag::TagsList(
        {
            {Tag::Date, QDate(2001, 01, 01)},
            {Tag::Time, QTime(10,0,0)},
            {Tag::Event, QString("Some event")}
        }),
        Tag::TagsList(
        {
            {Tag::Date, QDate(2001, 01, 01)},
            {Tag::Time, QTime(10,0,0)},
            {Tag::Event, QString("")}
        }),
        Tag::TagsList(
        {
            {Tag::Date, QDate(2001, 01, 01)},
            {Tag::Time, QTime(11,0,0)},
            {Tag::Event, QString("Another event")}
        })
    ));
}


TYPED_TEST(QueryTests, filterPhotosByTime)
{
     // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto response = this->m_backend->query(
        Query::commonFragments() +
        QStringLiteral(
        R"(
            query {
                photos(tags: {time: {eq: "10:00:00"}}) {
                    id
                    tags{
                        ...TagsFields
                    }
                }
            }
        )"));

    constexpr std::size_t expectedPhotos = 2;
    const auto tags = this-> template readTags<expectedPhotos>(response);

    EXPECT_THAT(tags, UnorderedElementsAre(
        Tag::TagsList(
        {
            {Tag::Date, QDate(2001, 01, 01)},
            {Tag::Time, QTime(10,0,0)},
            {Tag::Event, QString("Some event")}
        }),
        Tag::TagsList(
        {
            {Tag::Date, QDate(2001, 01, 01)},
            {Tag::Time, QTime(10,0,0)},
            {Tag::Event, QString("")}
        })
    ));
}


TYPED_TEST(QueryTests, filterPhotosWithPHash)
{
     // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(test_db);

    const auto response = this->m_backend->query(
        QStringLiteral(
        R"(
            query {
                photos(hasPHash: true) {
                    id
                    path
                }
            }
        )"));

    const std::vector<Photo::DataDelta> photos = ResponseParser::parsePhotosQueryResponse(response);

    const auto getPath = qOverload<const Photo::DataDelta &>(&Photo::getPath);
    EXPECT_THAT(photos,
        UnorderedElementsAre(ResultOf(getPath, Eq("/some/path2.jpeg")),
                             ResultOf(getPath, Eq("/some/path3.jpeg")))
    );
}
