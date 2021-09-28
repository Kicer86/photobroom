
#include <QDate>
#include <core/tag.hpp>

#include "database/backends/memory_backend/memory_backend.hpp"
#include "database_tools/json_to_backend.hpp"
#include "database_tools/common_backend_operations.hpp"
#include "unit_tests_utils/mock_backend.hpp"
#include "unit_tests_utils/sample_db_with_groups.json.hpp"


using testing::Eq;
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


TEST(JsonToBackendTest, groupsImport)
{
    Database::MemoryBackend backend;
    JsonToBackend converter(backend);

    converter.append(GroupsDB::db);

    const std::vector<Group::Id> groups = backend.groupOperator().listGroups();

    ASSERT_THAT(groups.size(), Eq(2));

    const auto group1Members = backend.groupOperator().membersOf(groups.front());
    const auto group2Members = backend.groupOperator().membersOf(groups.back());

    EXPECT_THAT(group1Members.size(), Eq(6));
    EXPECT_THAT(group2Members.size(), Eq(5));
}
