
/*
    Wraper for iterator over smart pointers collection.
    Behaves as iterator over raw pointers.
*/


#ifndef PTR_ITERATOR_HPP
#define PTR_ITERATOR_HPP

#include <type_traits>


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
class ptr_iterator: public T::const_iterator
{
    public:
        typedef typename T::iterator::iterator_category iterator_category;
        typedef typename T::iterator::value_type        value_type;
        typedef typename T::iterator::difference_type   difference_type;
        typedef typename T::iterator::pointer           pointer;
        typedef typename T::iterator::reference         reference;

        ptr_iterator(const typename T::const_iterator& iterator): T::const_iterator(iterator) {}
        ~ptr_iterator() {}

        const typename T::value_type::element_type* operator*() const
        {
            return (*this)->get();
        }
};

#endif
