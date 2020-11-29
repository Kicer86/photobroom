/*
 * Photo Broom - photos management tool.
    Behaves as iterator over raw pointers.
*/


#ifndef PTR_ITERATOR_HPP
#define PTR_ITERATOR_HPP

#include <type_traits>

#include "iterator_wrapper.hpp"
#include "generic_concepts.hpp"


/*
    Wraper for iterator over smart pointers collection.
    Behaves as iterator over raw pointers.
*/


#ifdef CONCEPTS_SUPPORTED

template<typename T>
concept SmartPointerContainer = requires(T p)
{
    requires Container<T>;
    requires SmartPointer<typename T::value_type>;
};

template<SmartPointerContainer T>
struct SmartPtrAccessor
{
    typename T::value_type::pointer operator()(const typename T::const_iterator& v) const
    {
        return v->get();
    }
};


template<SmartPointerContainer T>
using ptr_iterator = iterator_wrapper<typename T::value_type::pointer, typename T::const_iterator, SmartPtrAccessor<T>>;

#else

template<typename T>
struct SmartPtrAccessor
{
    typename T::value_type::pointer operator()(const typename T::const_iterator& v) const
    {
        return v->get();
    }
};

template<typename T>
using ptr_iterator = iterator_wrapper<typename T::value_type::pointer, typename T::const_iterator, SmartPtrAccessor<T>>;

#endif

#endif
