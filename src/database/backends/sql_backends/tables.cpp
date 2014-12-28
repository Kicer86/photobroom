
#include "tables.hpp"

#include <configuration/constants.hpp>

#include "table_definition.hpp"

namespace Database
{
        //check for proper sizes
        static_assert(sizeof(int) >= 4, "int is smaller than MySQL's equivalent");

        const char db_version[] = "0.01";

        TableDefinition
        table_versionHistory(TAB_VER_HIST,
                             {
                                 { "id", ColDefinition::Type::ID },
                                   "version DECIMAL(4,2) NOT NULL",       //xx.yy
                                   "date TIMESTAMP NOT NULL"
                             }
        );

        TableDefinition
        table_photos(TAB_PHOTOS,
                     {
                         { "id", ColDefinition::Type::ID },
                           "path VARCHAR(1024) NOT NULL",
                           "store_date TIMESTAMP NOT NULL"
                         },
                     {
                         { "ph_path", "INDEX", "(path)" }     //1024 limit required by MySQL
                     }
        );


        TableDefinition
        table_tag_names(TAB_TAG_NAMES,
                        {
                            { "id", ColDefinition::Type::ID },
                              QString("name VARCHAR(%1) NOT NULL").arg(Configuration::Constraints::database_tag_name_len),
                              "type INT NOT NULL"
                        },
                        {
                            { "tn_name", "UNIQUE INDEX", "(name)" }    //.arg(Consts::Constraints::database_tag_name_len)} required by MySQL
                        }
        );

        TableDefinition
        table_tags(TAB_TAGS,
                   {
                       { "id", ColDefinition::Type::ID },
                         QString("value VARCHAR(%1)").arg(Configuration::Constraints::database_tag_value_len),
                         "name_id INTEGER NOT NULL",
                         "photo_id INTEGER NOT NULL",
                         "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)",
                         "FOREIGN KEY(name_id) REFERENCES " TAB_TAG_NAMES "(id)"
                   }
        );

        TableDefinition
        table_thumbnails(TAB_THUMBS,
                         {
                             { "id", ColDefinition::Type::ID },
                               "photo_id INTEGER NOT NULL",
                               "data BLOB",
                               "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)"
                             },
                         {
                             { "th_photo_id", "UNIQUE INDEX", "(photo_id)" }  //one thumbnail per photo
                         }
        );


        TableDefinition
        table_hashes(TAB_HASHES,
                         {
                             { "id", ColDefinition::Type::ID },
                               "photo_id INTEGER NOT NULL",
                               "hash VARCHAR(256) NOT NULL",
                               "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)"
                         },
                         {
                             { "ha_hash", "INDEX", "(hash)"  },
                         }
        );

        //set of flags used internally
        TableDefinition
        table_flags(TAB_FLAGS,
                    {
                        { "id", ColDefinition::Type::ID },
                          "photo_id INTEGER NOT NULL",
                          "staging_area INT NOT NULL",
                          "tags_loaded INT NOT NULL",
                          "hash_loaded INT NOT NULL",
                          "thumbnail_loaded INT NOT NULL",
                          "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)"
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
                                     table_hashes,
                                     table_flags};
}
