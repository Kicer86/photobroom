
#ifndef CONTAINERS_UTILS
#define CONTAINERS_UTILS

#include "generic_concepts.hpp"

#ifndef CONCEPTS_SUPPORTED
#define Container typename
#endif

template<Container T>
const typename T::value_type& front(const T& container)
{
    return *container.begin();
}

template<Container T>
const typename T::value_type& back(const T& container)
{
    return *container.rbegin();
}

#ifndef CONCEPTS_SUPPORTED
#undef Container
#endif

#endif
