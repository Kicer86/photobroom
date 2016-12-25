
#ifndef DATABASE_TABLES_HPP
#define DATABASE_TABLES_HPP

#include <map>

#define TAB_VER            "version"
#define TAB_PHOTOS         "photos"
#define TAB_TAGS           "tags"
#define TAB_THUMBS         "thumbnails"
#define TAB_SHA256SUMS     "sha256sums"
#define TAB_FLAGS          "flags"
#define TAB_GEOMETRY       "geometry"
#define TAB_GROUPS         "groups"
#define TAB_GROUPS_MEMBERS "groups_members"

#define FLAG_STAGING_AREA  "staging_area"
#define FLAG_TAGS_LOADED   "tags_loaded"
#define FLAG_SHA256_LOADED "sha256_loaded"
#define FLAG_THUMB_LOADED  "thumbnail_loaded"
#define FLAG_GEOM_LOADED   "geometry_loaded"

namespace Database
{
    struct TableDefinition;
    extern std::map<std::string, TableDefinition> tables;
    extern const char db_version[];
}

#endif
