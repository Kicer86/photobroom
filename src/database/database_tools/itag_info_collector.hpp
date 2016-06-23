
#ifndef ITAG_INFO_COLLECTOR_HPP
#define ITAG_INFO_COLLECTOR_HPP

#include <core/tag.hpp>


struct ITagInfoCollector
{
    virtual ~ITagInfoCollector() {}

    virtual const std::set<TagValue>& get(const TagNameInfo &) const = 0;
};

#endif
