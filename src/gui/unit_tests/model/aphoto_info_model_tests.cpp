
#include <gmock/gmock.h>

#include "database/backends/memory_backend/memory_backend.hpp"
#include "database/database_tools/json_to_backend.hpp"
#include "unit_tests_utils/fake_photo_info.hpp"
#include "unit_tests_utils/mock_database.hpp"
#include "unit_tests_utils/sample_db.json.hpp"
#include "unit_tests_utils/mock_db_utils.hpp"
#include "utils/model_index_utils.hpp"

#include "flat_model.hpp"


using testing::Invoke;
using testing::ReturnRef;
using testing::NiceMock;
using testing::_;

// tests for implementations of APhotoInfoModel
template<typename T>
class APhotoInfoModelTest: public testing::Test
{
    public:
        APhotoInfoModelTest()
        {
            ON_CALL(db, execute).WillByDefault(Invoke([this](std::unique_ptr<Database::IDatabase::ITask>&& task)
            {
                task->run(backend);
            }));

            ON_CALL(db, backend).WillByDefault(ReturnRef(backend));

            ON_CALL(db, utils).WillByDefault(ReturnRef(utils));

            ON_CALL(utils, getPhotoFor(_)).WillByDefault(Invoke([this](const Photo::Id& id) -> IPhotoInfo::Ptr
            {
                const auto data = backend.getPhoto(id);
                auto photoPtr = std::make_shared<FakePhotoInfo>(data);

                return photoPtr;
            }));

            model.setDatabase(&db);
        }

        ~APhotoInfoModelTest()
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
        NiceMock<MockDatabase> db;
        NiceMock<MockUtils> utils;

};

using ModelTypes = testing::Types<FlatModel>;
TYPED_TEST_SUITE(APhotoInfoModelTest, ModelTypes);


TYPED_TEST(APhotoInfoModelTest, noEntriesWhenEmptyDatabase)
{
    EXPECT_EQ(this->model.rowCount({}), 0);
}


TYPED_TEST(APhotoInfoModelTest, photoDetailsForIndex)
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
        const Photo::Data& data = this->model.getPhotoData(idx);

        EXPECT_TRUE(data.id.valid());
    }
}
