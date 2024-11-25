
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <magic_enum/magic_enum.hpp>
#include <cmath>


template<typename T, typename R, const R T::*member>
R extract(const T& type)
{
    return type.*member;
}


// from https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template<class T>
bool almost_equal(T x, T y, int ulp = 2)
requires (std::numeric_limits<T>::is_integer == false)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::fabs(x-y) <= std::numeric_limits<T>::epsilon() * std::fabs(x+y) * ulp
        // unless the result is subnormal
        || std::fabs(x-y) < std::numeric_limits<T>::min();
}


namespace details
{
    template<auto... args, typename F>
    void unfold(F&& op)
    {
        (op(std::integral_constant<decltype(args), args>{}), ...);
    }

    template<auto arr, typename IS = decltype(std::make_index_sequence<arr.size()>())> struct Generator;

    template<auto arr, std::size_t... I>
    struct Generator<arr, std::index_sequence<I...>>
    {
        void operator()(auto op)
        {
            unfold<arr[I]...>(op);
        }
    };
}

/**
 * @brief call op for each value of enum E
 */
template<typename E>
void for_each(auto op)
{
    constexpr auto enum_values = magic_enum::enum_values<E>();
    details::Generator<enum_values>{}(op);
}

#endif
