
#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED


template<typename T, typename R, const R T::*member>
R extract(const T& type)
{
    return type.*member;
}

#endif // UTILS_HPP_INCLUDED
