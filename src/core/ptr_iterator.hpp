
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
        ptr_iterator(const typename T::const_iterator& ptr_iterator): m_ptr_iterator(ptr_iterator) {}
        ~ptr_iterator() {}

        ptr_iterator<T>& operator++()
        {
            ++m_ptr_iterator;

            return *this;
        }

        int operator-(const ptr_iterator<T>& other) const
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
