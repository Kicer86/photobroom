
#ifndef LAZY_PTR_HPP_INCLUDED
#define LAZY_PTR_HPP_INCLUDED

#include <memory>

template<typename T, typename C>
class lazy_ptr
{
    public:
        template<typename CC>
        lazy_ptr(CC&& constructor)
            : m_constructor(constructor)
        {

        }

        T* operator->()
        {
            return get();
        }

        T& operator*()
        {
            return *get();
        }

    private:
        std::unique_ptr<T> m_object;
        C& m_constructor;

        T* get()
        {
            if (m_object.get() == nullptr)
                m_object.reset( m_constructor() );

            return m_object.get();
        }
};

#endif // LAZY_PTR_HPP_INCLUDED
