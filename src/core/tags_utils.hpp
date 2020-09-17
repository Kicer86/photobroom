
#ifndef TAGS_UTILS_HPP_INCLUDED
#define TAGS_UTILS_HPP_INCLUDED


#include "tag.hpp"
#include "core_export.h"


namespace Tag
{
    CORE_EXPORT std::chrono::milliseconds timestamp(const TagsList &);
}

#endif // TAGS_UTILS_HPP_INCLUDED
