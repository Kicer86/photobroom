
#ifndef GENERIC_CONCEPTS
#define GENERIC_CONCEPTS

#include <map>
#include <unordered_map>


template<typename T>
concept SmartPointer = requires(T p)
{
    typename T::pointer;

    { p.operator*() };
    { p.get() };
    { std::is_pointer<decltype(*p)>::value == true };
};


template<typename T>
concept Iterator = requires(T i)
{
    typename T::iterator_category;
    typename T::value_type;
    typename T::difference_type;
    typename T::pointer;
    typename T::reference;

    { i.operator*() };
    { i.operator->() };
};


template<typename T>
concept Container = requires(T p)
{
    typename T::const_iterator;
    typename T::value_type;

    requires Iterator<typename T::const_iterator>;
    requires Iterator<typename T::iterator>;

    { p.begin() };
    { p.end()   };
};


// https://stackoverflow.com/a/64088175/1749713
template<typename T>
concept map_type =
    std::same_as<T, std::map<typename T::key_type, typename T::mapped_type, typename T::key_compare, typename T::allocator_type>> ||
    std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, typename T::allocator_type>>;


#endif
