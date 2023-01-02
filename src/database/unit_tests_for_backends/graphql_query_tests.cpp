
#include "database_tools/json_to_backend.hpp"
#include "unit_tests_utils/sample_db.json.hpp"

#include "common.hpp"


template<typename T>
struct QueryTests: DatabaseTest<T>
{

};

TYPED_TEST_SUITE(QueryTests, BackendTypes);


TYPED_TEST(QueryTests, getPhotos)
{
     // fill backend with sample data
    Database::JsonToBackend converter(*this->m_backend);
    converter.append(SampleDB::db1);

    const auto response = this->m_backend->query(QStringLiteral(
        R"(
            {
                photos {
                    id
                }
            }
        )"));

    EXPECT_EQ(response, R"({"data":{"photos":[{"id":"1"},{"id":"2"},{"id":"3"}]}})");
}
