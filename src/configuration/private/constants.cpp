
#include "constants.hpp"

namespace Consts
{
	const unsigned int Constraints::database_tag_name_len = 256;
	const unsigned int Constraints::database_tag_value_len = 2048;

	const char DefaultTags::tag_people[] = "People";
	const char DefaultTags::tag_place[]  = "Place";
	const char DefaultTags::tag_date[]   = "Date";
	const char DefaultTags::tag_event[]  = "Event";

	const std::vector<const char *> DefaultTags::tags_list({ tag_people, tag_place, tag_date, tag_event });
}
