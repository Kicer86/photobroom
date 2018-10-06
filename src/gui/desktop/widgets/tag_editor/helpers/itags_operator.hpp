

#ifndef ITAGS_OPEARTOR_HPP
#define ITAGS_OPEARTOR_HPP

#include <core/tag.hpp>
#include <database/iphoto_info.hpp>

struct ITagsOperator
{
    virtual ~ITagsOperator() = default;

    virtual void operateOn(const std::vector<IPhotoInfo::Ptr> &) = 0;

    virtual Tag::TagsList getTags() const = 0;

    virtual void setTag(const TagNameInfo &, const TagValue &) = 0;
    virtual void setTags(const Tag::TagsList &) = 0;
    virtual void updateTag(const TagNameInfo& name, const TagValue &) = 0;
};


#endif // ITAGS_OPEARTOR_HPP
