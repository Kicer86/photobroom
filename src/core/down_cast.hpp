
#ifndef DOWN_CAST_HPP
#define DOWN_CAST_HPP

#include <cassert>

template<typename T, typename R>
T down_cast(R* base)
{
    assert(dynamic_cast<T>(base) != nullptr);

    return static_cast<T>(base);
}

#endif
