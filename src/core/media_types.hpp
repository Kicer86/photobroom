

#ifndef MEDIA_TYPES_HPP
#define MEDIA_TYPES_HPP

#include "core_export.h"

class QString;

namespace MediaTypes
{
    CORE_EXPORT bool isImageFile(const QString &);
    CORE_EXPORT bool isAnimatedImageFile(const QString &);
    CORE_EXPORT bool isVideoFile(const QString &);
}

#endif
