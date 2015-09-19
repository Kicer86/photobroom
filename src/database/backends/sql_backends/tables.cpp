
#include "tables.hpp"

#include <configuration/constants.hpp>

#include "table_definition.hpp"

namespace Database
{
        //check for proper sizes
        static_assert(sizeof(int) >= 4, "int is smaller than MySQL's equivalent");

        const char db_version[] = "1";

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
                         {  "store_date", "TIMESTAMP NOT NULL"    }
                     },
                     {
                         { "ph_path", "INDEX", "(path)" }     //1024 limit required by MySQL
                     }
        );


        TableDefinition
        table_tag_names(TAB_TAG_NAMES,
                        {
                            { "id", "", ColDefinition::Purpose::ID },
                            { "name", QString("VARCHAR(%1) NOT NULL").arg(ConfigConsts::Constraints::database_tag_name_len) },
                            { "type", "INT NOT NULL" }
                        },
                        {
                            { "tn_name", "UNIQUE INDEX", "(name)" }    //.arg(Consts::Constraints::database_tag_name_len)} required by MySQL
                        }
        );

        TableDefinition
        table_tags(TAB_TAGS,
                   {
                       { "id", "", ColDefinition::Purpose::ID },
                       { "value", QString("VARCHAR(%1)").arg(ConfigConsts::Constraints::database_tag_value_len) },
                       { "name_id", "INTEGER NOT NULL"     },
                       { "photo_id", "INTEGER NOT NULL"    },
                       { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "ON DELETE CASCADE"   },
                       { "FOREIGN KEY(name_id) REFERENCES " TAB_TAG_NAMES "(id)", "ON DELETE CASCADE" }
                   }
        );

        TableDefinition
        table_thumbnails(TAB_THUMBS,
                         {
                             { "id", "", ColDefinition::Purpose::ID                      },
                             { "photo_id", "INTEGER NOT NULL"                            },
                             { "data", "BLOB"                                            },
                             { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "ON DELETE CASCADE" }
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
                             { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "ON DELETE CASCADE" }
                         },
                         {
                             { "ha_photo_id", "UNIQUE INDEX", "(photo_id)" },               //one sha per photo
                             { "ha_sha256", "INDEX", "(sha256)"            },
                         }
        );

        //set of flags used internally
        TableDefinition
        table_flags(TAB_FLAGS,
                    {
                        { "id", "", ColDefinition::Purpose::ID   },
                        { "photo_id",  "INTEGER NOT NULL"     },
                        { FLAG_STAGING_AREA,  "INT NOT NULL" },
                        { FLAG_TAGS_LOADED,   "INT NOT NULL" },
                        { FLAG_SHA256_LOADED, "INT NOT NULL" },
                        { FLAG_THUMB_LOADED,  "INT NOT NULL" },
                        { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "ON DELETE CASCADE" }
                    },
                    {
                        { "fl_photo_id", "UNIQUE INDEX", "(photo_id)" }  //one set of flags per photo
                    }
        );


        //all tables
        TableDefinition tables[7] = {table_versionHistory,
                                     table_photos,
                                     table_tag_names,
                                     table_tags,
                                     table_thumbnails,
                                     table_sha256sums,
                                     table_flags};
}
