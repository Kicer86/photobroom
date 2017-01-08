
/*
    Wraper for iterator over smart pointers collection.
    Behaves as iterator over raw pointers.
*/


#ifndef PTR_ITERATOR_HPP
#define PTR_ITERATOR_HPP

template<typename T>
class ptr_iterator
{
    public:
        typedef typename T::iterator::iterator_category iterator_category;
        typedef typename T::iterator::value_type        value_type;
        typedef typename T::iterator::difference_type   difference_type;
        typedef typename T::iterator::pointer           pointer;
        typedef typename T::iterator::reference         reference;

        ptr_iterator(const typename T::const_iterator& iterator): m_ptr_iterator(iterator) {}
        ~ptr_iterator() {}

        ptr_iterator<T>& operator++()
        {
            ++m_ptr_iterator;

            return *this;
        }

        difference_type operator-(const ptr_iterator<T>& other) const
        {
            return m_ptr_iterator - other.m_ptr_iterator;
        }

        bool operator==(const ptr_iterator<T>& other) const
        {
            return m_ptr_iterator == other.m_ptr_iterator;
        }

        bool operator!=(const ptr_iterator<T>& other) const
        {
            return m_ptr_iterator != other.m_ptr_iterator;
        }

        const typename T::value_type::element_type* operator*() const
        {
            return m_ptr_iterator->get();
        }

    private:
        typename T::const_iterator m_ptr_iterator;
};

#endif
