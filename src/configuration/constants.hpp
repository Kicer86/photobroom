
#ifndef CONFIGURATION_CONSTANTS_HPP
#define CONFIGURATION_CONSTANTS_HPP

#include <vector>

namespace Consts
{

    namespace Constraints
    {
        extern const unsigned int database_tag_name_len;
        extern const unsigned int database_tag_value_len;
    }

    namespace DefaultTags
    {
        extern const char tag_people[];
        extern const char tag_place[];
        extern const char tag_date[];
        extern const char tag_event[];

        extern const std::vector<const char *> tags_list;
    }

}
#endif
