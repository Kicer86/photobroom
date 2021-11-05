
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED


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

#endif // UTILS_HPP_INCLUDED
