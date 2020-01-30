
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

        explicit lazy_ptr(C& constructor)
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
        // When C can be copied, then keep C by value.
        // Otherwise keep C as reference
        typedef typename std::conditional<std::is_copy_constructible<C>::value, C, C&>::type CType;

        std::unique_ptr<T> m_object;
        CType m_constructor;

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

#endif // LAZY_PTR_HPP_INCLUDED
