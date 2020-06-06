
#include "database_tools/json_to_backend.hpp"
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
