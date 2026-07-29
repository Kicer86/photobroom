#include <gtest/gtest.h>

#include <core/enum_reflection.hpp>

namespace
{
    enum class TestEnum
    {
        First,
        Second,
    };
}

TEST(EnumReflection, returnsNamesAndEmptyNameForUnknownValue)
{
    EXPECT_EQ(reflection::enum_values<TestEnum>.size(), 2);
    EXPECT_EQ(reflection::enum_name(TestEnum::First), "First");
    EXPECT_EQ(reflection::enum_name(TestEnum::Second), "Second");
    EXPECT_TRUE(reflection::enum_name(static_cast<TestEnum>(-1)).empty());
}
