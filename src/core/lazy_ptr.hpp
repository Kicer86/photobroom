
#ifndef LAZY_PTR_HPP_INCLUDED
#define LAZY_PTR_HPP_INCLUDED

#include <memory>

template<typename T, typename C>
class lazy_ptr
{
    public:
        lazy_ptr()
            : m_constructor()
        {

        }

        explicit lazy_ptr(const C& constructor)
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
        C m_constructor;

        // used when C::operator() returns raw pointer
        struct PtrCreator
        {
            std::unique_ptr<T> operator()(C& constructor) const
            {
                return std::unique_ptr<T>(constructor());
            }
        };

        // used when C::operator() returns value
        struct CopyCreator
        {
            std::unique_ptr<T> operator()(C& constructor) const
            {
                return std::make_unique<T>(constructor());
            }
        };

        T* get()
        {
            if (m_object.get() == nullptr)
            {
                typedef typename std::conditional<std::is_pointer<decltype(m_constructor())>::value, PtrCreator, CopyCreator>::type Creator;
                const Creator creator;

                m_object = creator(m_constructor);
            }

            return m_object.get();
        }
};


template<typename T, typename C>
lazy_ptr<T, C> make_lazy_ptr(const C& c)
{
    return lazy_ptr<T, C>(c);
}

#endif // LAZY_PTR_HPP_INCLUDED
