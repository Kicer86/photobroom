
#ifndef MAP_ITERATOR_HPP
#define MAP_ITERATOR_HPP

#include "iterator_wrapper.hpp"


/*
 * Wraper for std::map::iterator which itself behaves as iterator over map's keys
 */

template<typename T>
struct MapKeyAccessor
{
    typename T::value_type::first_type operator()(const typename T::const_iterator& v) const
    {
        return v->first;
    }
};

template<typename T>
struct MapValueAccessor
{
    typename T::value_type::second_type operator()(const typename T::const_iterator& v) const
    {
        return v->second;
    }
};


template<typename T>
using key_map_iterator = iterator_wrapper<typename T::value_type::first_type, typename T::const_iterator, MapKeyAccessor<T>>;

template<typename T>
using value_map_iterator = iterator_wrapper<typename T::value_type::second_type, typename T::const_iterator, MapValueAccessor<T>>;

#endif
