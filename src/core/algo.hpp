
#ifndef ALGO_HPP
#define ALGO_HPP

#include <set>
#include <map>

namespace Algo
{
    struct KeyIntersector
    {
        KeyIntersector() {}
        ~KeyIntersector() {}

        template<typename T>
        bool equal(const T& v1, const T& v2) const
        {
            return v1->first == v2->first;
        }
    };

    struct KeyValueIntersector
    {
        KeyValueIntersector() {}
        ~KeyValueIntersector() {}

        template<typename T>
        bool equal(const T& v1, const T& v2) const
        {
            return v1->first == v2->first && v1->second == v2->second;
        }
    };
        
    template<typename KeyType, typename ValType, typename Intersector = KeyIntersector>
    std::map<KeyType, ValType> map_intersection(const std::map<KeyType, ValType> &m1, const std::map<KeyType, ValType> &m2)
    {
        std::map<KeyType, ValType> results;
        Intersector intersector;
        
        auto it1 = m1.begin();
        auto it2 = m2.begin();
        
        while (it1 != m1.end() && it2 != m2.end())
        {
            if (intersector.equal(it1, it2))
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
