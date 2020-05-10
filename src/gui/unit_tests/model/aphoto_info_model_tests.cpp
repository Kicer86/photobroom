
#include <gmock/gmock.h>

#include <db_data_model.hpp>
#include <flat_model.hpp>

#include <unit_tests_utils/mock_database.hpp>

// tests for implementations of APhotoInfoModel
class APhotoInfoModelTest: testing::Test
{
    public:
        APhotoInfoModelTest()
        {
            model1.setDatabase(&db);
            model2.setDatabase(&db);
        }

        ~APhotoInfoModelTest()
        {
        }

    private:
        FlatModel model1;
        DBDataModel model2;
        MockDatabase db;

        std::vector<APhotoInfoModel *> models = { &model1, &model2 };
};
