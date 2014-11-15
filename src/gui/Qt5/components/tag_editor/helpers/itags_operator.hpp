

#ifndef ITAGS_OPEARTOR_HPP
#define TAGGS_OPEARTOR_HPP

#include <core/tag.hpp>
#include <database/iphoto_info.hpp>

struct ITagsOperator
{
    virtual ~ITagsOperator() {}

    virtual void operateOn(const std::vector<IPhotoInfo::Ptr> &) = 0;

    virtual Tag::TagsList getTags() const = 0;

    virtual void setTag(const TagNameInfo &, const Tag::ValuesSet &) = 0;
    virtual void setTag(const TagNameInfo &, const TagValueInfo &) = 0;
    virtual void setTags(const Tag::TagsList &) = 0;
    virtual void updateTag(const QString& name, const TagValueInfo &) = 0;
};


#endif //TAGGS_OPEARTOR_HPP
