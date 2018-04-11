
#ifndef LAZY_CACHE_HPP
#define LAZY_CACHE_HPP

#include <functional>
#include <mutex>
#include <optional>
#include <vector>

template<typename T>
class LazyCache
{
    public:
        typedef std::function<void(T)> Getter;
        typedef std::function<void(const Getter &)> GetterCallback;

        LazyCache(const GetterCallback& g): m_getter(g) {}

        template<typename R>
        void get(const R& callback)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            if (m_value)
                callback(*m_value);
            else
            {
                m_wating.push_back(callback);

                lock.unlock();
                get();
            }
        }

    private:
        GetterCallback m_getter;
        std::mutex m_mutex;
        std::optional<T> m_value;
        std::vector<std::function<void(const T &)>> m_wating;

        void assign(const T& v)
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                m_value = v;
            }

            for(const auto w: m_wating)
                w(*m_value);
        }

        void get()
        {
            auto set = std::bind(&LazyCache<T>::assign, this, std::placeholders::_1);
            m_getter(set);
        }
};


#endif
