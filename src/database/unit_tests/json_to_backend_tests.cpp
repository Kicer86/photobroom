
#include <QDate>
#include <core/tag.hpp>

#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/mock_backend.hpp"


using testing::StrictMock;
using testing::Return;

using Database::JsonToBackend;


TEST(JsonToBackendTest, emptyJson)
{
    StrictMock<MockBackend> backend;        // expect no calls

    JsonToBackend converter(backend);
    converter.append(QString());
    converter.append({});
    converter.append("{}");
}

TEST(JsonToBackendTest, photo)
{
    Tag::TagsList tags = {
        { TagTypes::Date, QDate::fromString("01.01.2001") },
        { TagTypes::Time, QTime::fromString("12:34:56", "HH:mm:ss") },
        { TagTypes::Event, QString("qwerty") },
        { TagTypes::Place, QString("asdfgh") }
    };

    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/some/path");
    photo.insert<Photo::Field::Tags>(tags);

    std::vector<Photo::DataDelta> photos = { photo };

    MockBackend backend;
    EXPECT_CALL(backend, addPhotos(photos)).WillOnce(Return(true));

    JsonToBackend converter(backend);

    converter.append(
        R"(
        {
            "photos": [
                {
                    "path": "/some/path",
                    "tags": {
                        "date":  "01.01.2001",
                        "time":  "12:34:56",
                        "event": "qwerty",
                        "place": "asdfgh"
                    }
                }
            ]
        }
        )"
    );

}
