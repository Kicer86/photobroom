
#ifndef CONFIGURATION_CONSTANTS_HPP
#define CONFIGURATION_CONSTANTS_HPP

#include <vector>

#include "configuration_export.h"

namespace Consts
{

	struct CONFIGURATION_EXPORT Constraints
    {
        static const unsigned int database_tag_name_len;
        static const unsigned int database_tag_type_len;
        static const unsigned int database_tag_value_len;
	};

	struct CONFIGURATION_EXPORT DefaultTags
    {
		static const char tag_people[];
		static const char tag_place[];
        static const char tag_time[];
		static const char tag_date[];
		static const char tag_event[];

		static const std::vector<const char *> tags_list;
	};

}
#endif
