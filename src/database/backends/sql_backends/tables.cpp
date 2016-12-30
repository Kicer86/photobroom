
#include "tables.hpp"

#include <configuration/constants.hpp>

#include "table_definition.hpp"

namespace Database
{
        //check for proper sizes
        static_assert(sizeof(int) >= 4, "int is smaller than MySQL's equivalent");

        const char db_version[] = "2";

        TableDefinition
        table_versionHistory(TAB_VER,
                             {
                                 { "version", "INT NOT NULL" }
                             }
        );


        TableDefinition
        table_photos(TAB_PHOTOS,
                     {
                         { "id", "", ColDefinition::Purpose::ID   },
                         { "path", "VARCHAR(1024) NOT NULL"       },
                         { "store_date", "TIMESTAMP NOT NULL"     }
                     },
                     {
                         { "ph_path", "INDEX", "(path)" }     //1024 limit required by MySQL
                     }
        );


        TableDefinition
        table_tags(TAB_TAGS,
                   {
                       { "id", "", ColDefinition::Purpose::ID },
                       { "value", QString("VARCHAR(%1)").arg(ConfigConsts::Constraints::database_tag_value_len) },
                       { "name", "INTEGER NOT NULL"        },
                       { "photo_id", "INTEGER NOT NULL"    },
                       { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", ""   },
                   },
                   {
                       { "tg_id", "UNIQUE INDEX", "(id)" },
                       { "tg_photo_id", "INDEX", "(photo_id)" }
                   }
        );


        TableDefinition
        table_thumbnails(TAB_THUMBS,
                         {
                             { "id", "", ColDefinition::Purpose::ID                      },
                             { "photo_id", "INTEGER NOT NULL"                            },
                             { "data", "BLOB"                                            },
                             { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
                         },
                         {
                             { "th_photo_id", "UNIQUE INDEX", "(photo_id)" }  //one thumbnail per photo
                         }
        );


        TableDefinition
        table_sha256sums(TAB_SHA256SUMS,
                         {
                             { "id", "", ColDefinition::Purpose::ID                      },
                             { "photo_id INTEGER NOT NULL", ""                           },
                             { "sha256 CHAR(32) NOT NULL", ""                            },
                             { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
                         },
                         {
                             { "ha_photo_id", "UNIQUE INDEX", "(photo_id)" },               //one sha per photo
                             { "ha_sha256", "INDEX", "(sha256)"            },
                         }
        );


        TableDefinition
        table_geometry(TAB_GEOMETRY,
                       {
                           { "id", "", ColDefinition::Purpose::ID   },
                           { "photo_id INTEGER NOT NULL", ""        },
                           { "width", "INT NOT NULL"                },
                           { "height", "INT NOT NULL"               },
                           { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
                       },
                       {
                           { "g_id", "UNIQUE INDEX", "(id)"             },
                           { "g_photo_id", "UNIQUE INDEX", "(photo_id)" },
                       }
        );


        //set of flags used internally
        TableDefinition
        table_flags(TAB_FLAGS,
                    {
                        { "id", "", ColDefinition::Purpose::ID   },
                        { "photo_id",  "INTEGER NOT NULL"    },
                        { FLAG_STAGING_AREA,  "INT NOT NULL" },
                        { FLAG_TAGS_LOADED,   "INT NOT NULL" },
                        { FLAG_SHA256_LOADED, "INT NOT NULL" },
                        { FLAG_THUMB_LOADED,  "INT NOT NULL" },
                        { FLAG_GEOM_LOADED,   "INT NOT NULL" },
                        { FLAG_ROLE,          "INT NOT NULL" },
                        { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
                    },
                    {
                        { "fl_photo_id", "UNIQUE INDEX", "(photo_id)" }  //one set of flags per photo
                    }
        );

        // information about groups of photos
        TableDefinition
        table_groups(TAB_GROUPS,
                    {
                        { "id", "", ColDefinition::Purpose::ID   },
                        { "representative_id",  "INTEGER NOT NULL"     },
                        { "FOREIGN KEY(representative_id) REFERENCES " TAB_PHOTOS "(id)", "" }
                    }
        );

        // information about groups' members
        TableDefinition
        table_groups_members(TAB_GROUPS_MEMBERS,
                    {
                        { "id", "", ColDefinition::Purpose::ID   },
                        { "group_id", "INTEGER NOT NULL"     },
                        { "photo_id", "INTEGER NOT NULL"     },
                        { "FOREIGN KEY(group_id) REFERENCES " TAB_GROUPS "(id)", "" },
                        { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
                    }
        );

        //all tables
        std::map<std::string, TableDefinition> tables =
        {
            { TAB_VER,            table_versionHistory },
            { TAB_PHOTOS,         table_photos },
            { TAB_TAGS,           table_tags },
            { TAB_THUMBS,         table_thumbnails },
            { TAB_SHA256SUMS,     table_sha256sums },
            { TAB_FLAGS,          table_flags },
            { TAB_GEOMETRY,       table_geometry },
            { TAB_GROUPS,         table_groups },
            { TAB_GROUPS_MEMBERS, table_groups_members },
        };
}
