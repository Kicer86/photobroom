
#ifndef CONFIGURATION_CONSTANTS_HPP
#define CONFIGURATION_CONSTANTS_HPP

#include <vector>

#include "configuration_export.h"

namespace Configuration
{

    struct CONFIGURATION_EXPORT Constraints
    {
        static const unsigned int database_tag_name_len;
        static const unsigned int database_tag_value_len;
    };

    namespace BasicKeys
    {
        const char* const configLocation = "Application::ConfigDir";
        const char* const thumbnailWidth = "Database::ThumbnailWidth";
    }

}
#endif
