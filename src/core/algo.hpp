
#ifndef ALGO_HPP
#define ALGO_HPP

#include <set>
#include <map>

namespace Algo
{     
    
    template<typename KeyType, typename ValType, typename... Args>
    std::map<KeyType, ValType> map_intersection(const std::map<KeyType, ValType> &m1, Args... args)
    {
        std::map<KeyType, ValType> results = map_intersection<KeyType, ValType>(args...);
        
        results = map_intersection<KeyType, ValType>(m1, results);
        
        return results;
    }
    
    
    template<typename KeyType, typename ValType>
    std::map<KeyType, ValType> map_intersection(const std::map<KeyType, ValType> &m1, const std::map<KeyType, ValType> &m2)
    {
        std::map<KeyType, ValType> results;
        
        auto it1 = m1.begin();
        auto it2 = m2.begin();
        
        while (it1 != m1.end() && it2 != m2.end())
        {
            if (*it1 == *it2)
            {
                results.insert(*it1);
                
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
