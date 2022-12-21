
#include <gmock/gmock.h>
#include <QAbstractItemModelTester>

#include "database/backends/memory_backend/memory_backend.hpp"
#include "database/database_tools/json_to_backend.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "utils/model_index_utils.hpp"

#include "flat_model.hpp"


using testing::Invoke;
using testing::ReturnRef;
using testing::NiceMock;
using testing::_;

// tests for implementations of APhotoInfoModel
template<typename T>
class APhotoDataModelTest: public testing::Test
{
    public:
        APhotoDataModelTest()
        {
            ON_CALL(db, execute).WillByDefault(Invoke([this](std::unique_ptr<Database::IDatabase::ITask>&& task)
            {
                task->run(backend);
            }));

            ON_CALL(db, backend).WillByDefault(ReturnRef(backend));

            model.setDatabase(&db);
        }

        ~APhotoDataModelTest()
        {
        }

        void loadDB(const char* json)
        {
            Database::JsonToBackend converter(backend);
            converter.append(json);
        }

        T model;

    private:
        Database::MemoryBackend backend;
        NiceMock<DatabaseMock> db;

};

using ModelTypes = testing::Types<FlatModel>;
TYPED_TEST_SUITE(APhotoDataModelTest, ModelTypes);


TYPED_TEST(APhotoDataModelTest, qtTest)
{
    QAbstractItemModelTester tester(&this->model, QAbstractItemModelTester::FailureReportingMode::Fatal);
}


TYPED_TEST(APhotoDataModelTest, noEntriesWhenEmptyDatabase)
{
    EXPECT_EQ(this->model.rowCount({}), 0);
}


TYPED_TEST(APhotoDataModelTest, photoDetailsForIndex)
{
    this->loadDB(SampleDB::db1);

    utils::forEach(this->model, [this](const QModelIndex& idx)
    {
        if (this->model.canFetchMore(idx))
            this->model.fetchMore(idx);
    });

    const int rows = this->model.rowCount({});
    ASSERT_EQ(rows, 3);

    for (int r = 0; r < 3; r++)
    {
        const QModelIndex idx = this->model.index(r, 0, {});
        const auto& data = this->model.getPhotoData(idx);

        EXPECT_TRUE(data.getId().valid());
    }
}
