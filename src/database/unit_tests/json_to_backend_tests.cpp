
#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/mock_backend.hpp"


using testing::StrictMock;
using Database::JsonToBackend;

TEST(JsonToBackendTest, emptyJson)
{
    StrictMock<MockBackend> backend;        // expect no calls

    JsonToBackend converter(backend);
    converter.append(QString());
    converter.append({});
    converter.append("{}");
}
