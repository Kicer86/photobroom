
#ifndef CALLBACK_PTR_HPP
#define CALLBACK_PTR_HPP

#include <functional>
#include <memory>

#include <OpenLibrary/putils/ts_resource.hpp>


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
