
/*
    Wraper for std::map::iterator which itself behaves as iterator over map's keys
*/



#ifndef MAP_ITERATOR_HPP
#define MAP_ITERATOR_HPP

#include "iterator_wrapper.hpp"

template<typename T>
struct MapKeyAccessor
{
    typename T::value_type::first_type operator()(const typename T::value_type& v) const
    {
        return v.first;
    }
};


template<typename T>
using key_map_iterator = iterator_wrapper<typename T::value_type, typename T::const_iterator, MapKeyAccessor<T>>;

#endif
