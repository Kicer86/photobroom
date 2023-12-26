
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

    make_lazy_ptr<int>([&constructor]() {return constructor();} );
}


TEST(LazyPtrTest, onlyOneConstructionWhenNeeded)
{
    ConstructorMock<int> constructor;
    EXPECT_CALL(constructor, build).Times(1).WillOnce(Return(new int));

    lazy_ptr ptr = make_lazy_ptr<int>([&constructor]() {return constructor();} );

    *ptr = 0;
    *ptr = 5;
}


TEST(LazyPtrTest, onlyOneConstructionWhenNeededForComplexType)
{
    ConstructorMock<std::pair<int, double>> constructor;
    EXPECT_CALL(constructor, build).Times(1).WillOnce(Return(new std::pair<int, double>));

    lazy_ptr ptr = make_lazy_ptr<std::pair<int, double>>([&constructor]() {return constructor();} );

    ptr->first = 0;
    ptr->second = 5.0;
}


TEST(LazyPtrTest, qtPropertiesConstructor)
{
    QObject obj;

    using ObjectTakingQStringInContructor = QString;

    auto lazy_ptr = make_lazy_ptr<ObjectTakingQStringInContructor, QString>(&obj, "objectName");
    obj.setObjectName("hello");

    EXPECT_EQ(*lazy_ptr, "hello");
}
