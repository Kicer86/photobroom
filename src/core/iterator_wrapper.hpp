
#ifndef ITERATOR_WRAPPER_HPP
#define ITERATOR_WRAPPER_HPP

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

        iterator operator++()
        {
            return B::operator++();
        }

        template<typename O>
        bool operator==(const O& other) const
        {
            const B* base = this;
            return *base == other;
        }

    private:
        T m_operation;
};

#endif
