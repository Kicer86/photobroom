
#include <gmock/gmock.h>

#include "lazy_cache.hpp"

typedef std::function<void(int)> CacheSetter;
typedef std::function<void( const CacheSetter & )> CacheSetterCallback;

typedef LazyCache<int, CacheSetterCallback> Cache;

using testing::Return;
using testing::_;

struct Getter
{
    MOCK_CONST_METHOD0(get, int());

    void operator()(const std::function<void(int)>& set) const
    {
        set(get());
    }
};


struct IntRetriever
{
    MOCK_CONST_METHOD1(extract, void(int));
};

TEST(LazyCacheTest, ifGetterIsCalledOnce)
{
    IntRetriever retriever;
    EXPECT_CALL(retriever, extract(5)).Times(3);

    Getter getter;
    EXPECT_CALL(getter, get()).WillOnce(Return(5));

    Cache cache([&getter](const std::function<void(int)>& c){ getter(c); });

    cache.get([&retriever](int v){ retriever.extract(v); });
    cache.get([&retriever](int v){ retriever.extract(v); });
    cache.get([&retriever](int v){ retriever.extract(v); });
}


