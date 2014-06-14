
#ifndef DATABASE_TABLES_HPP
#define DATABASE_TABLES_HPP

#define TAB_TAG_NAMES "tag_names"
#define TAB_VER_HIST  "version_history"
#define TAB_PHOTOS    "photos"
#define TAB_TAGS      "tags"
#define TAB_THUMBS    "thumbnails"
#define TAB_FLAGS     "flags"

namespace Database
{
    struct TableDefinition;
    extern TableDefinition tables[6];
    extern const char db_version[];
}

#endif
