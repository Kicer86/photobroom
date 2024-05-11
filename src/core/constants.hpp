
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


namespace Parameters
{
    const QSize databaseThumbnailSize(400, 400);
}

#endif
