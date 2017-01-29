
/*
    Wraper for iterator over smart pointers collection.
    Behaves as iterator over raw pointers.
*/


#ifndef PTR_ITERATOR_HPP
#define PTR_ITERATOR_HPP

#include <type_traits>

#include "iterator_wrapper.hpp"

template<typename T>
concept bool SmartPointer()
{
    return requires(T p)
    {
        { p.operator*() };
        { p.get() };
        { std::is_pointer<decltype(*p)>::value == true };
    };
}


template<typename T>
concept bool Iterator()
{
    return requires(T i)
    {
        typename T::iterator_category;
        typename T::value_type;
        typename T::difference_type;
        typename T::pointer;
        typename T::reference;

        { i.operator*() };
        { i.operator->() };
    };
}


template<typename T>
concept bool Container()
{
    return requires(T p)
    {
        typename T::const_iterator;

        requires Iterator<typename T::const_iterator>();
        requires Iterator<typename T::iterator>();

        { p.begin() };
        { p.end()   };

    };
}


template<typename T>
concept bool SmartPointerContainer()
{
    return requires(T p)
    {
        requires Container<T>();

        { *p.begin() } -> SmartPointer;
    };
}


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

#endif
