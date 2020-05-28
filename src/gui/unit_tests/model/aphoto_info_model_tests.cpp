
#include <gmock/gmock.h>

#include <database/backends/memory_backend/memory_backend.hpp>
#include <unit_tests_utils/mock_database.hpp>

#include "db_data_model.hpp"
#include "flat_model.hpp"


using testing::Invoke;
using testing::Return;
using testing::NiceMock;

// tests for implementations of APhotoInfoModel
template<typename T>
class APhotoInfoModelTest: public testing::Test
{
    public:
        APhotoInfoModelTest()
        {
            ON_CALL(db, execute).WillByDefault(Invoke([this](std::unique_ptr<Database::IDatabase::ITask>&& task)
            {
                task->run(&backend);
            }));

            ON_CALL(db, backend).WillByDefault(Return(&backend));

            model.setDatabase(&db);
        }

        ~APhotoInfoModelTest()
        {
        }

        T model;

    private:
        NiceMock<Database::MemoryBackend> backend;
        NiceMock<MockDatabase> db;
};

using ModelTypes = testing::Types<FlatModel, DBDataModel>;
TYPED_TEST_SUITE(APhotoInfoModelTest, ModelTypes);


TYPED_TEST(APhotoInfoModelTest, noEntriesWhenEmptyDatabase)
{
    EXPECT_EQ(this->model.rowCount({}), 0);
}
