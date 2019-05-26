
#ifndef CACHE_HPP
#define CACHE_HPP

#include <chrono>
#include <unordered_map>


// Cache container which removes old items.
template<typename K, typename V, typename Hash>
class Cache
{
    public:
        Cache(int size):
            m_size(size)
        {
        }

        void insert(const K& k, const V& v)
        {
            Data data = {v, std::chrono::steady_clock::now()};

            m_data.insert_or_assign(k, data);

            reduce();
        }

        void erase(const K& k)
        {
            m_data.erase(k);
        }

        std::optional<V> find(const K& k) const
        {
            std::optional<V> result;

            auto it = m_data.find(k);
            if (it != m_data.end())
                result = it->second.value;

            return result;
        }

    private:
        struct Data
        {
            V value;
            std::chrono::steady_clock::time_point time;
        };

        std::unordered_map<K, Data, Hash> m_data;
        int m_size;

        void reduce()
        {
            while (m_data.size() > m_size)
            {
                auto drop = m_data.begin();

                for (auto it = m_data.begin(); it != m_data.end(); ++it)
                {
                    if (it->second.time < drop->second.time)
                        drop = it;
                }

                if (drop != m_data.end())
                    m_data.erase(drop);
            }
        }
};

#endif
