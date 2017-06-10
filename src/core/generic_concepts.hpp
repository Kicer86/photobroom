
#ifndef GENERIC_CONCEPTS
#define GENERIC_CONCEPTS


#ifdef CONCEPTS_SUPPORTED

template<typename T>
concept bool SmartPointer()
{
    return requires(T p)
    {
        typename T::pointer;

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
        typename T::value_type;

        requires Iterator<typename T::const_iterator>();
        requires Iterator<typename T::iterator>();

        { p.begin() };
        { p.end()   };

    };
}

#endif

#endif
