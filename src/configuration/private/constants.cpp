
#include "constants.hpp"

#include <QtGlobal>

namespace Consts
{
	const unsigned int Constraints::database_tag_name_len = 64;
	const unsigned int Constraints::database_tag_value_len = 2048;

	const char DefaultTags::tag_people[] = QT_TRANSLATE_NOOP("Default Tag Name", "People");
	const char DefaultTags::tag_place[]  = QT_TRANSLATE_NOOP("Default Tag Name", "Place");
	const char DefaultTags::tag_date[]   = QT_TRANSLATE_NOOP("Default Tag Name", "Date");
    const char DefaultTags::tag_time[]   = QT_TRANSLATE_NOOP("Default Tag Name", "Time");
	const char DefaultTags::tag_event[]  = QT_TRANSLATE_NOOP("Default Tag Name", "Event");

	const std::vector<const char *> DefaultTags::tags_list({ tag_people, tag_place, tag_date, tag_event });
}
