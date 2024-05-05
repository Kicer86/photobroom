
#include <gmock/gmock.h>

#include "models/aheavy_list_model.hpp"

using testing::Invoke;
using testing::Return;
using testing::NiceMock;


class AHeavyListModelMock: public AHeavyListModel<int>
{
public:
    MOCK_METHOD(int, rowCount, (const QModelIndex &), (const, override));
    MOCK_METHOD(QVariant, data, (const QModelIndex &, int), (const, override));

    MOCK_METHOD(void, loadData, (const std::stop_token &stopToken, StoppableTaskCallback<std::vector<int>>), (override));
    MOCK_METHOD(void, applyRows, (const QList<int> &, AHeavyListModel::ApplyToken), (override));
};


TEST(AHeavyListModelTest, initialState)
{
    AHeavyListModelMock model;

    EXPECT_EQ(model.state(), AHeavyListModelMock::Idle);
}


TEST(AHeavyListModelTest, process)
{
    NiceMock<AHeavyListModelMock> model;
    EXPECT_CALL(model, loadData).WillOnce(
        Invoke(
            [&model](const std::stop_token, auto callback)
            {
                EXPECT_EQ(model.state(), AHeavyListModelMock::Fetching);
                callback(std::vector<int>(555));
            }
        )
    );

    ON_CALL(model, rowCount).WillByDefault(Return(0));

    model.reload();
    EXPECT_EQ(model.state(), AHeavyListModelMock::Loaded);

    model.clear();
    EXPECT_EQ(model.state(), AHeavyListModelMock::Idle);
}
