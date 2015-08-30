
#ifndef CONFIGURATION_CONSTANTS_HPP
#define CONFIGURATION_CONSTANTS_HPP

#include <vector>

#include "configuration_export.h"

namespace ConfigConsts
{

    struct CONFIGURATION_EXPORT Constraints
    {
        static const unsigned int database_tag_name_len;
        static const unsigned int database_tag_value_len;
    };

    struct CONFIGURATION_EXPORT BasicKeys
    {
        [[deprecated("should be moved to database module")]] static const char* const thumbnailWidth;
    };

}
#endif
