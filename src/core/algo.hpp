
#ifndef ALGO_HPP
#define ALGO_HPP

#include <set>
#include <map>

namespace Algo
{   
    
    namespace Private
    {
        template<typename SetT, typename ValueT>
        static void inserter(SetT &set, const ValueT &value);
        
        template<typename T>
        void inserter(std::set<T>& set, const T& value)
        {
            set.insert(value);
        }
        
        template<typename T>
        void inserter(std::set<T>& set, const std::set<T> &value)
        {
            set.insert(value.begin(), value.end());
        }

    }
        
    template<typename RetType, typename KeyType, typename ValType1, typename ValType2>
    std::map<KeyType, std::set<RetType>> map_intersection(const std::map<KeyType, ValType1> &m1, const std::map<KeyType, ValType2> &m2)
    {
        std::map<KeyType, std::set<RetType>> results;
        
        auto it1 = m1.begin();
        auto it2 = m2.begin();
        
        while (it1 != m1.end() && it2 != m2.end())
        {
            if (it1->first == it2->first)
            {
                std::set<RetType> mergedResults;
                Private::inserter(mergedResults, it1->second);
                Private::inserter(mergedResults, it2->second);
                
                results.insert(std::make_pair(it1->first, mergedResults));
                
                ++it1;
                ++it2;
            }
            else if (it1->first < it2->first)
                ++it1;
            else 
                ++it2;            
        }
        
        return results;
    }
    
}

#endif
