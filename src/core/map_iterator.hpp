
/*
    Wraper for std::map::iterator which itself behaves as iterator over map's keys
*/



#ifndef MAP_ITERATOR_HPP
#define MAP_ITERATOR_HPP

template<typename T>
class key_map_iterator
{
    public:
        key_map_iterator(const typename T::const_iterator& map_iterator): m_map_iterator(map_iterator) {}
        ~key_map_iterator() {}

        key_map_iterator<T>& operator++()
        {
            ++m_map_iterator;

            return *this;
        }

        bool operator==(const key_map_iterator<T>& other) const
        {
            return m_map_iterator == other.m_map_iterator;
        }

        bool operator!=(const key_map_iterator<T>& other) const
        {
            return m_map_iterator != other.m_map_iterator;
        }

        const typename T::key_type& operator*() const
        {
            return m_map_iterator->first;
        }

    private:
        typename T::const_iterator m_map_iterator;
};

#endif
