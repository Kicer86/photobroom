
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
    Photo::DataDelta photo;
    photo.insert<Photo::Field::Path>("/some/path");
    std::vector<Photo::DataDelta> photos = { photo };

    MockBackend backend;
    EXPECT_CALL(backend, addPhotos(photos)).WillOnce(Return(true));

    JsonToBackend converter(backend);

    converter.append(
        R"(
        {
            "photos": [
                {
                    "path": "/some/path"
                }
            ]
        }
        )"
    );

}
