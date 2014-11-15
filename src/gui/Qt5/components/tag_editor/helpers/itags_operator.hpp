

#ifndef ITAGS_OPEARTOR_HPP
#define TAGGS_OPEARTOR_HPP

#include <core/tag.hpp>

struct ITagsOperator
{
    virtual ~ITagsOperator() {}

    virtual Tag::TagsList getTags() const = 0;

    virtual void setTag(const TagNameInfo &, const Tag::ValuesSet &) = 0;
    virtual void setTag(const TagNameInfo &, const TagValueInfo &) = 0;
    virtual void setTags(const Tag::TagsList &) = 0;
};


#endif //TAGGS_OPEARTOR_HPP
