#include <gmock/gmock.h>

#include <QObject>
#include <functional>
#include <vector>

#include "property_awaiter.hpp"
#include "property_awaiter_test_object.hpp"


TEST(PropertyAwaiterTest, callsAfterAllPropertiesChangeInOrder)
{
    TestObject object;
    int calls = 0;
    const std::vector<const char*> properties = {"first", "second"};

    PropertyAwaiter awaiter(&object, properties, [&calls]() { ++calls; });

    object.setFirst(1);
    EXPECT_EQ(calls, 0);

    object.setSecond(2);
    EXPECT_EQ(calls, 1);
}


TEST(PropertyAwaiterTest, callsAfterAllPropertiesChangeOutOfOrder)
{
    TestObject object;
    int calls = 0;
    const std::vector<const char*> properties = {"first", "second", "third"};

    PropertyAwaiter awaiter(&object, properties, [&calls]() { ++calls; });

    object.setThird(1);
    EXPECT_EQ(calls, 0);

    object.setFirst(2);
    EXPECT_EQ(calls, 0);

    object.setSecond(3);
    EXPECT_EQ(calls, 1);
}


TEST(PropertyAwaiterTest, ignoresRepeatedChangesUntilAllPropertiesReported)
{
    TestObject object;
    int calls = 0;
    const std::vector<const char*> properties = {"first", "second"};

    PropertyAwaiter awaiter(&object, properties, [&calls]() { ++calls; });

    object.setFirst(1);
    object.setFirst(2);
    EXPECT_EQ(calls, 0);

    object.setSecond(3);
    EXPECT_EQ(calls, 1);
}

