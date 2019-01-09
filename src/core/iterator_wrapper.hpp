
#ifndef ITERATOR_WRAPPER_HPP
#define ITERATOR_WRAPPER_HPP

#include <iterator>


template<typename R, typename B, typename T>
class iterator_wrapper: private B
{
        typedef iterator_wrapper<R, B, T> iterator;

    public:
        iterator_wrapper(): B(), m_operation()
        {

        }

        iterator_wrapper(const B& base): B(base), m_operation()
        {

        }

        iterator_wrapper(const iterator_wrapper &) = default;

        ~iterator_wrapper()
        {
        }

        R operator*() const
        {
            return m_operation(*this);
        }

        R operator*()
        {
            return m_operation(*this);
        }

        int operator-(const B& other) const
        {
            const B* base = this;
            return *base - other;
        }

        int operator-(const iterator& other) const
        {
            const B* base = this;
            return *base - other;
        }

        iterator& operator++()
        {
            B::operator++();
            return *this;
        }

        iterator& operator--()
        {
            B::operator++();
            return *this;
        }

        bool operator==(const B& other) const
        {
            const B* base = this;
            return *base == other;
        }

        bool operator==(const iterator& other) const
        {
            const B* base = this;
            return *base == other;
        }

        bool operator!=(const B& other) const
        {
            const B* base = this;
            return *base != other;
        }

        bool operator!=(const iterator& other) const
        {
            const B* base = this;
            return *base != other;
        }

        iterator& operator+=(int diff)
        {
            B::operator+=(diff);

            return *this;
        }

    private:
        T m_operation;
};

template<>
template<typename R, typename B, typename T>
struct std::iterator_traits<iterator_wrapper<R, B, T>>: std::iterator_traits<B> { };

#endif
