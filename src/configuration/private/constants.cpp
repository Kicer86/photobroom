
#include "constants.hpp"

namespace Consts
{
    namespace Constraints
    {
        const unsigned int database_tag_name_len = 256;
        const unsigned int database_tag_value_len = 2048;
    }

    namespace DefaultTags
    {
        const char tag_people[] = "People";
        const char tag_place[]  = "Place";
        const char tag_date[]   = "Date";
        const char tag_event[]  = "Event";

        const std::vector<const char *> tags_list({tag_people, tag_place, tag_date, tag_event});
    }
}
