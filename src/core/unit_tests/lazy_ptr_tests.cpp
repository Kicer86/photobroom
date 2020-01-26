
#include <gmock/gmock.h>

#include "lazy_ptr.hpp"


using testing::Return;

template<typename T>
class ConstructorMock
{
    public:
        MOCK_METHOD(T*, build, ());

        T* operator()()
        {
            return build();
        }
};


TEST(LazyPtrTest, noConstructionWhenNotNeeded)
{
    ConstructorMock<int> constructor;
    EXPECT_CALL(constructor, build).Times(0);

    lazy_ptr<int, ConstructorMock<int>> ptr(constructor);
}


TEST(LazyPtrTest, onlyOneConstructionWhenNeeded)
{
    ConstructorMock<int> constructor;
    EXPECT_CALL(constructor, build).Times(1).WillOnce(Return(new int));

    lazy_ptr<int, ConstructorMock<int>> ptr(constructor);

    *ptr = 0;
    *ptr = 5;
}


TEST(LazyPtrTest, onlyOneConstructionWhenNeededForComplexType)
{
    ConstructorMock<std::pair<int, double>> constructor;
    EXPECT_CALL(constructor, build).Times(1).WillOnce(Return(new std::pair<int, double>));

    lazy_ptr<std::pair<int, double>, ConstructorMock<std::pair<int, double>>> ptr(constructor);

    ptr->first = 0;
    ptr->second = 5.0;
}
