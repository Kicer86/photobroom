

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

    virtual void setTag(const TagNameInfo &, const TagValue &) = 0;             // set tag to value
    virtual void setTags(const Tag::TagsList &) = 0;                            // set list of tags
    virtual void updateTag(const TagNameInfo& name, const TagValue &) = 0;      // update value of existing tag (if it is different than provided)
};


#endif // ITAGS_OPEARTOR_HPP
