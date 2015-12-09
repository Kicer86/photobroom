
#ifndef CALLBACK_PTR_HPP
#define CALLBACK_PTR_HPP

#include <memory>

#include <OpenLibrary/putils/ts_resource.hpp>


template<typename T>
class [[deprecated]] callback_ptr_ctrl final
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
class [[deprecated]] callback_ptr
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


struct safe_callback_data
{
    std::mutex mutex;
    bool callbackAlive;

    safe_callback_data(): mutex(), callbackAlive(true) {}
};



template<typename T>
class safe_callback
{
    public:
        safe_callback(const std::shared_ptr<safe_callback_data>& data, const T& callback): m_data(data), m_callback(callback) {}
        safe_callback(const safe_callback<T> &) = default;

        safe_callback& operator=(const safe_callback<T> &) = default;

        virtual ~safe_callback()
        {
        }

        template<typename... Args>
        void operator() (Args... args)
        {
            std::lock_guard<std::mutex> lock(m_data->mutex);

            if (m_data->callbackAlive == true)
                m_callback(args...);
        }

    private:
        std::shared_ptr<safe_callback_data> m_data;

        T m_callback;
};


class safe_callback_ctrl final
{
    public:
        safe_callback_ctrl(): m_data()
        {
            setup();
        }

        safe_callback_ctrl(const safe_callback_ctrl &) = delete;

        ~safe_callback_ctrl()
        {
            reset();
        }

        template<typename R, typename T>
        auto make_safe_callback(const T& callback) const
        {
            safe_callback<T> callbackPtr(m_data, callback);
            std::function<R> fun(callbackPtr);

            return fun;
        }

        safe_callback_ctrl& operator=(const safe_callback_ctrl &) = delete;

        void invalidate()
        {
            reset();      // dissolve all connections
            setup();      // create new one
        }

    private:
        template<typename>
        friend class safe_callback;

        std::shared_ptr<safe_callback_data> m_data;

        void setup()
        {
            m_data = std::make_shared<safe_callback_data>();
        }

        void reset()
        {
            // mark all safe callbacks as invalid
            {
                // lock resource
                std::lock_guard<std::mutex> lock(m_data->mutex);

                // mark resource as dead
                m_data->callbackAlive = false;
            }

            // detach from existing safe callbacks
            m_data.reset();
        }
};


#endif
