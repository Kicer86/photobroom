
#ifndef CONTAINERS_UTILS
#define CONTAINERS_UTILS

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>

#include "generic_concepts.hpp"


template<Container T>
const typename T::value_type& front(const T& container)
{
    return *container.begin();
}

template<Container T>
const typename T::value_type& back(const T& container)
{
    return *container.rbegin();
}

template<Container T>
typename T::value_type take_front(T& container)
{
    typename T::value_type v( std::move(container.front()) );
    container.pop_front();

    return v;
}

template<Container T>
typename T::value_type take_back(T& container)
{
    typename T::value_type v( std::move(container.back()) );
    container.pop_back();

    return v;
}


template<typename MapT,
            typename RemovedT,
            typename ChangedT,
            typename AddedT>
void compare(const MapT& lhs, const MapT& rhs,
                RemovedT rem_inserter,
                ChangedT chg_inserter,
                AddedT add_inserter)
{
    typedef typename MapT::value_type Pair;
    typedef std::tuple<typename MapT::key_type,
                       typename MapT::mapped_type,
                       typename MapT::mapped_type> Changed;

    // find *keys* which exist only in lhs
    std::set_difference(lhs.cbegin(), lhs.cend(),
                        rhs.cbegin(), rhs.cend(),
                        rem_inserter, [](const Pair& l, const Pair& r)
    {
        return l.first < r.first;
    });

    // find *keys* which exist only in rhs
    std::set_difference(rhs.cbegin(), rhs.cend(),
                        lhs.cbegin(), lhs.cend(),
                        add_inserter, [](const Pair& l, const Pair& r)
    {
        return l.first < r.first;
    });

    // find *keys* which exist in both collections
    std::vector<Pair> intersection;
    std::set_intersection(lhs.cbegin(), lhs.cend(),
                            rhs.cbegin(), rhs.cend(),
                            std::back_inserter(intersection), [](const Pair& l, const Pair& r)
    {
        return l.first < r.first;
    });

    for(const Pair& lhs_data: intersection)
    {
        auto it = rhs.find(lhs_data.first);
        assert(it != rhs.cend());

        if (lhs_data.second != it->second)
        {
            Changed changed(lhs_data.first, lhs_data.second, it->second);

            chg_inserter = changed;
            chg_inserter++;
        }
    }

}

template<typename T>
std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    const std::size_t size = std::max(lhs.size(), rhs.size());

    std::vector<T> result(size);

    for(std::size_t i = 0; i < size; i++)
    {
        const T l = i < lhs.size()? lhs[i]: T();
        const T r = i < rhs.size()? rhs[i]: T();

        result[i] = l + r;
    }

    return result;
}

template<typename T>
std::vector<T>& operator+=(std::vector<T>& lhs, const std::vector<T>& rhs)
{
    lhs = lhs + rhs;

    return lhs;
}


template<typename T, typename P>
std::vector<T> operator/(const std::vector<T>& lhs, const P& rhs)
{
    const std::size_t size = lhs.size();
    std::vector<T> result(size);

    for(std::size_t i = 0; i < size; i++)
        result[i] = lhs[i] / rhs;

    return result;
}


template<typename T, typename P>
std::vector<T>& operator/=(std::vector<T>& lhs, const P& rhs)
{
    lhs = lhs / rhs;

    return lhs;
}

#endif
