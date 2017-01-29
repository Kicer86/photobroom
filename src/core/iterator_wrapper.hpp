
#ifndef ITERATOR_WRAPPER_HPP
#define ITERATOR_WRAPPER_HPP

template<typename R, typename B, typename T>
struct iterator_wrapper: B
{
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

    private:
        T m_operation;
};

#endif
