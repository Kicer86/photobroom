
#ifndef LAZY_PTR_HPP_INCLUDED
#define LAZY_PTR_HPP_INCLUDED

#include <memory>
#include <functional>
#include <QObject>
#include <QVariant>


template<typename T>
class lazy_ptr
{
    public:
        lazy_ptr()
            : m_constructor()
        {

        }

        template<typename C>
        explicit lazy_ptr(const C& constructor)
            : m_constructor()
        {
            if constexpr (std::is_pointer_v<decltype(constructor())>)
                m_constructor = [constructor]() -> std::unique_ptr<T>
                {
                    return std::unique_ptr<T>(constructor());
                };
            else
                m_constructor = [constructor]() -> std::unique_ptr<T>
                {
                    return std::make_unique<T>(constructor());
                };
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
        using C = std::function<std::unique_ptr<T>()>;
        std::unique_ptr<T> m_object;
        C m_constructor;

        T* get()
        {
            if (m_object.get() == nullptr)
                m_object = m_constructor();

            return m_object.get();
        }
};


/**
 * @brief Construct lazy_ptr<T> which will be initialized with result of C() called on first use.
 */
template<typename T, typename C>
requires std::invocable<C>
lazy_ptr<T> make_lazy_ptr(const C& c)
{
    return lazy_ptr<T>(c);
}

namespace lazy_ptr_impl
{
    template<typename T>
    T getQtPropertyValue(QObject* obj, const char* name)
    {
        const QVariant valueVariant = obj->property(name);

        return valueVariant.value<T>();
    }
}

/**
 * @brief Construct lazy_ptr<T> which will be initialized (on first use) with values of Qt's properties passed under Args... which are expected to be const char *
 *
 * Use case for this function is a class which's member depends on values of properties marked as REQUIRED.
 * As such properties are not set at construction time, but soon after, some members may need to wait for
 * them to be set, and therfore need to me constructed later.
 */
template<typename T, typename... Types, typename... Args>
requires (std::is_same_v<Args, const char *> && ...)
auto make_lazy_ptr(QObject* obj, Args... args)
{
    assert(obj != nullptr);
    static_assert(sizeof...(Types) == sizeof...(Args), "Number of arguments and their types needs to be equal");

    auto c = [obj, args...]() -> T
    {
        return T(lazy_ptr_impl::getQtPropertyValue<Types>(obj, args)...);
    };

    return make_lazy_ptr<T>(c);
}

#endif
