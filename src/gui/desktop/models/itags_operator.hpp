

#ifndef ITAGS_OPEARTOR_HPP
#define ITAGS_OPEARTOR_HPP

#include <core/tag.hpp>
#include <database/iphoto_info.hpp>

// Interface for tags operations on set of photos.

struct ITagsOperator
{
    virtual ~ITagsOperator() = default;

    virtual void operateOn(const std::vector<IPhotoInfo::Ptr> &) = 0;           // scope

    virtual Tag::TagsList getTags() const = 0;                                  // list of common tags

    virtual void setTag(const Tag::Types &, const TagValue &) = 0;                // set tag to value
    virtual void setTags(const Tag::TagsList &) = 0;                            // set list of tags
    virtual void insert(const Tag::Types& name, const TagValue &) = 0;            // set or update exisiting tag
};


#endif // ITAGS_OPEARTOR_HPP
