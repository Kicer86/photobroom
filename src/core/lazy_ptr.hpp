
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

        // used when C::operator() returns pointer
        struct PtrCreator
        {
            T* operator()(C& constructor)
            {
                return constructor();
            }
        };

        // used when C::operator() returns value
        struct CopyCreator
        {
            T* operator()(C& constructor)
            {
                return new T(constructor());
            }
        };

        T* get()
        {
            if (m_object.get() == nullptr)
            {
                typedef typename std::conditional<std::is_pointer<decltype(m_constructor())>::value, PtrCreator, CopyCreator>::type Creator;
                Creator creator;

                m_object.reset( creator(m_constructor) );
            }

            return m_object.get();
        }
};

#endif // LAZY_PTR_HPP_INCLUDED
