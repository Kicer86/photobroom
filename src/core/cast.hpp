
#ifndef DOWN_CAST_HPP
#define DOWN_CAST_HPP

#include <cassert>
#include <type_traits>


template<typename T, typename R>
T down_cast(R* base)
{
    assert(dynamic_cast<T>(base) != nullptr);

    return static_cast<T>(base);
}


template<typename T, typename F>
T safe_cast(const F& from)
{
    static_assert(sizeof(F) <= sizeof(T));                          // TODO: handle size conversion when needed
    static_assert(std::is_nothrow_convertible_v<F, T>);

    if constexpr (std::is_signed_v<F> && std::is_unsigned_v<T>)     // cast from signed to unsigned
        assert(from >= 0);

    return static_cast<T>(from);
}

#endif
