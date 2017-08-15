

#ifndef MEDIA_TYPES_HPP
#define MEDIA_TYPES_HPP

#include <vector>

#include "core_export.h"

namespace MediaTypes
{
    // regexps for supported media files
    CORE_EXPORT extern const std::vector<const char *> images_extensions;
    CORE_EXPORT extern const std::vector<const char *> movies_extensions;

    CORE_EXPORT extern const std::vector<const char *> all_extensions;
}

#endif
