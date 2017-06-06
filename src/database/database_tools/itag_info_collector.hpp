
#ifndef ITAG_INFO_COLLECTOR_HPP
#define ITAG_INFO_COLLECTOR_HPP

#include <functional>

#include <core/tag.hpp>


struct ITagInfoCollector
{
    virtual ~ITagInfoCollector() {}

    virtual const std::deque<TagValue>& get(const TagNameInfo &) const = 0;
    virtual int registerChangeObserver( const std::function< void(const TagNameInfo &) > & ) = 0;
    virtual void unregisterChangeObserver(int) = 0;
};

#endif
