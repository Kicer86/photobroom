
#include "constants.hpp"

#include <QtGlobal>

namespace Consts
{
    namespace Constraints
    {
        const unsigned int database_tag_name_len = 256;
        const unsigned int database_tag_value_len = 2048;
    }

    namespace DefaultTags
    {
        const char tag_people[] = QT_TRANSLATE_NOOP("Default Tag Name", "People");
        const char tag_place[]  = QT_TRANSLATE_NOOP("Default Tag Name", "Place");
        const char tag_date[]   = QT_TRANSLATE_NOOP("Default Tag Name", "Date");
        const char tag_time[]   = QT_TRANSLATE_NOOP("Default Tag Name", "Time");
        const char tag_event[]  = QT_TRANSLATE_NOOP("Default Tag Name", "Event");

        const std::vector<const char *> tags_list({tag_people, tag_place, tag_date, tag_time, tag_event});
    }
}
