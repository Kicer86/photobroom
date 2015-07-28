
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
        template<typename TT>
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

                // remove resource from Data, so no other instance of Data will not be able to access it
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


#endif
