
#ifndef CONFIGURATION_CONSTANTS_HPP
#define CONFIGURATION_CONSTANTS_HPP

#include <vector>
#include <QSize>

#include "core_export.h"

namespace ConfigConsts
{

    struct CORE_EXPORT Constraints
    {
        static const unsigned int database_tag_name_len;
        static const unsigned int database_tag_value_len;
    };

}


namespace ExternalToolsConfigKeys
{
    [[deprecated]] const char* const convertPath = "tool_path::convert";
    const char* const aisPath     = "tool_path::align_image_stack";
    const char* const magickPath  = "tool_path::magick";
    const char* const ffmpegPath  = "tool_path::ffmpeg";
    const char* const ffprobePath = "tool_path::ffprobe";
}

namespace Parameters
{
    const QSize databaseThumbnailSize(400, 400);
}

#endif
