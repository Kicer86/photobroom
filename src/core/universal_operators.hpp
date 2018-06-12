
#ifndef UNIVERSAL_OPERATOR_HPP
#define UNIVERSAL_OPERATOR_HPP


template<typename T, typename P>
bool isEqual(const T& lhs, const T& rhs, const P& p)
{
    return lhs.*p == rhs.*p;
}


template<typename T, typename P>
bool isLess(const T& lhs, const T& rhs, const P& p)
{
    return lhs.*p < rhs.*p;
}


template<typename T, typename P, typename... Args>
bool isLess(const T& lhs, const T& rhs, const P& p, Args... args)
{
    if (isLess(lhs, rhs, p))
        return true;
    else if (isEqual(lhs, rhs, p))
        return isLess(lhs, rhs, args...);
    else
        return false;
}

#endif
