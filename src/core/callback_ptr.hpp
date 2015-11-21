
#ifndef CALLBACK_PTR_HPP
#define CALLBACK_PTR_HPP

#include <memory>

#include <OpenLibrary/putils/ts_resource.hpp>


template<typename T>
class callback_ptr_ctrl final
{
    public:
        callback_ptr_ctrl(T* callback): m_data(), m_callback(callback)
        {
            setup();
        }

        callback_ptr_ctrl(const callback_ptr_ctrl<T> &) = delete;

        ~callback_ptr_ctrl()
        {
            reset();
        }

        callback_ptr_ctrl& operator=(const callback_ptr_ctrl<T> &) = delete;

        void invalidate()
        {
            reset();      // dissolve all connections
            setup();      // create new one
        }

    private:
        template<typename>
        friend class callback_ptr;

        struct Data
        {
            ol::ThreadSafeResource<T *> m_callback;

            Data(T* callback): m_callback(callback) {}
        };

        std::shared_ptr<Data> m_data;
        T* m_callback;

        void setup()
        {
            m_data = std::make_shared<Data>(m_callback);
        }

        void reset()
        {
            {
                // lock resource
                auto callback_resource_locked = m_data->m_callback.lock();

                // remove resource from Data, so no other instance of Data will be able to access it
                *callback_resource_locked = nullptr;
            }

            // detach clear assocation with others
            m_data.reset();
        }
};


template<typename T>
class callback_ptr
{
    public:
        callback_ptr(const callback_ptr_ctrl<T>& ctrl): m_data(ctrl.m_data) {}
        callback_ptr(const callback_ptr<T> &) = delete;

        callback_ptr& operator=(const callback_ptr<T> &) = delete;

        virtual ~callback_ptr()
        {
        }

        typename ol::ThreadSafeResource<T *>::Accessor operator*()
        {
            auto access = m_data->m_callback.lock();

            return std::move(access);
        }

    private:
        std::shared_ptr<typename callback_ptr_ctrl<T>::Data> m_data;
};


struct callback_ptr_data
{
    ol::ThreadSafeResource<bool> m_callbackAlive;

    callback_ptr_data(): m_callbackAlive(true) {}
};



template<typename T>
class callback_ptr2
{
    public:
        callback_ptr2(const std::shared_ptr<callback_ptr_data>& data, const T& callback): m_data(data), m_callback(callback) {}
        callback_ptr2(const callback_ptr2<T> &) = default;

        callback_ptr2& operator=(const callback_ptr2<T> &) = default;

        virtual ~callback_ptr2()
        {
        }

        template<typename... Args>
        void operator() (Args... args)
        {
            auto access = m_data->m_callbackAlive.lock();

            if (*access == true)
                m_callback(args...);
        }

    private:
        std::shared_ptr<callback_ptr_data> m_data;

        T m_callback;
};


class callback_ptr_ctrl2 final
{
    public:
        callback_ptr_ctrl2(): m_data()
        {
            setup();
        }

        callback_ptr_ctrl2(const callback_ptr_ctrl2 &) = delete;

        ~callback_ptr_ctrl2()
        {
            reset();
        }

        template<typename R>
        std::function<R> make_safe_callback(const std::function<R>& callback)
        {
            callback_ptr2<std::function<R>> callbackPtr(m_data, callback);
            std::function<R> fun(callbackPtr);

            return fun;
        }

        callback_ptr_ctrl2& operator=(const callback_ptr_ctrl2 &) = delete;

        void invalidate()
        {
            reset();      // dissolve all connections
            setup();      // create new one
        }

    private:
        template<typename>
        friend class callback_ptr2;

        std::shared_ptr<callback_ptr_data> m_data;

        void setup()
        {
            m_data = std::make_shared<callback_ptr_data>();
        }

        void reset()
        {
            {
                // lock resource
                auto locked = m_data->m_callbackAlive.lock();

                // mark resource as dead
                *locked = false;
            }

            // detach clear assocation with others
            m_data.reset();
        }
};


#endif
