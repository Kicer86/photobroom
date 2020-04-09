
#include <gmock/gmock.h>
#include <QSignalSpy>

#include "desktop/models/flat_model.hpp"
#include "unit_tests_utils/mock_database.hpp"


class FlatModelTest: public testing::Test
{
    protected:
        FlatModel model;
        MockDatabase db;
};


TEST_F(FlatModelTest, resetAfterDBSet)
{
    QSignalSpy model_about_to_be_reset(&model, &FlatModel::modelAboutToBeReset);
    QSignalSpy model_reset(&model, &FlatModel::modelReset);

    model.setDatabase(nullptr);
    EXPECT_EQ(model_about_to_be_reset.count(), 1);
    EXPECT_EQ(model_reset.count(), 1);

    model.setDatabase(&db);
    EXPECT_EQ(model_about_to_be_reset.count(), 2);
    EXPECT_EQ(model_reset.count(), 2);
}
