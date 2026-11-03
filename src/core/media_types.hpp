

#ifndef MEDIA_TYPES_HPP
#define MEDIA_TYPES_HPP

#include "core_export.h"
#include "filesystem.hpp"


namespace MediaTypes
{
    CORE_EXPORT bool isImageFile(const Filesystem::Location& filename);
    CORE_EXPORT bool isAnimatedImageFile(const Filesystem::Location& filename);
    CORE_EXPORT bool isVideoFile(const Filesystem::Location& filename);
}

#endif
