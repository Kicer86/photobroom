
#include <gmock/gmock.h>

#include "models/aheavy_list_model.hpp"

using testing::Invoke;

class AHeavyListModelMock: public AHeavyListModel<int>
{
public:
    MOCK_METHOD(int, rowCount, (const QModelIndex &), (const, override));
    MOCK_METHOD(QVariant, data, (const QModelIndex &, int), (const, override));

    MOCK_METHOD(void, clearData, (), (override));
    MOCK_METHOD(void, loadData, (const std::stop_token &stopToken, StoppableTaskCallback<int>), (override));
    MOCK_METHOD(void, updateData, (const int &), (override));
    MOCK_METHOD(void, applyRows, (const QList<int> &, AHeavyListModel::ApplyToken), (override));
};


TEST(AHeavyListModelTest, initialState)
{
    AHeavyListModelMock model;

    EXPECT_EQ(model.state(), AHeavyListModelMock::Idle);
}


TEST(AHeavyListModelTest, process)
{
    AHeavyListModelMock model;
    EXPECT_CALL(model, clearData).Times(2);     // First one before Fetching, second one at the end of the test
    EXPECT_CALL(model, loadData).WillOnce(
        Invoke(
            [&model](const std::stop_token, auto callback)
            {
                EXPECT_EQ(model.state(), AHeavyListModelMock::Fetching);
                callback(555);
            }
        )
    );
    EXPECT_CALL(model, updateData(555));

    model.reload();
    EXPECT_EQ(model.state(), AHeavyListModelMock::Loaded);

    model.clear();
    EXPECT_EQ(model.state(), AHeavyListModelMock::Idle);
}
