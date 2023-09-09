
#include "tables.hpp"

#include <core/constants.hpp>

#include "table_definition.hpp"

namespace Database
{
        //check for proper sizes
        static_assert(sizeof(int) >= 4, "int is smaller than MySQL's equivalent");

        const int db_version = 6;

        TableDefinition
        table_versionHistory(TAB_VER,
        {
            { "version", "INT NOT NULL" }
        });


        TableDefinition
        table_photos(TAB_PHOTOS,
        {
            { "id", "", ColDefinition::Purpose::ID   },
            { "path", "VARCHAR(1024) NOT NULL"       },
            { "store_date", "TIMESTAMP NOT NULL"     }
        },
        {
            { "ph_path", "INDEX", "(path)" }     //1024 limit required by MySQL
        });


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
        });


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
        });


        TableDefinition
        table_blobs(TAB_BLOBS,
        {
            { "id", "", ColDefinition::Purpose::ID                      },
            { "photo_id", "INTEGER NOT NULL"                            },
            { "type", "INTEGER NOT NULL"                                },
            { "data", "BLOB"                                            },
            { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
        },
        {
            { "bb_photo_id", "UNIQUE INDEX", "(photo_id, type)" }
        });


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
        });


        //set of flags used internally
        TableDefinition
        table_flags(TAB_FLAGS,
        {
            { "id", "", ColDefinition::Purpose::ID   },
            { "photo_id",  "INTEGER NOT NULL"    },
            { FLAG_STAGING_AREA,  "INT NOT NULL" },
            { FLAG_TAGS_LOADED,   "INT NOT NULL" },
            { FLAG_GEOM_LOADED,   "INT NOT NULL" },
            { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
        },
        {
            { "fl_photo_id", "UNIQUE INDEX", "(photo_id)" }  //one set of flags per photo
        });

        // information about groups of photos
        TableDefinition
        table_groups(TAB_GROUPS,
        {
            { "id", "", ColDefinition::Purpose::ID   },
            { "representative_id",  "INTEGER NOT NULL"     },
            { "type",               "INTEGER"              },
            { "FOREIGN KEY(representative_id) REFERENCES " TAB_PHOTOS "(id)", "" }
        });

        // information about groups' members
        TableDefinition
        table_groups_members(TAB_GROUPS_MEMBERS,
        {
            { "id", "", ColDefinition::Purpose::ID   },
            { "group_id", "INTEGER NOT NULL"         },
            { "photo_id", "INTEGER NOT NULL"         },
            { "FOREIGN KEY(group_id) REFERENCES " TAB_GROUPS "(id)", "" },
            { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", "" }
        });

        // list of people
        TableDefinition
        table_people( TAB_PEOPLE_NAMES,
        {
            { "id", "", ColDefinition::Purpose::ID   },
            { "name", QString("VARCHAR(%1)").arg(ConfigConsts::Constraints::database_tag_value_len) },
        },
        {
            { "ppl_name", "UNIQUE INDEX", "(name)" },   // names should be unique
        });

        // list of people assigned to particular photo
        TableDefinition
        table_people_locations(TAB_PEOPLE,
        {
            { "id", "", ColDefinition::Purpose::ID },
            { "photo_id", "INTEGER NOT NULL"       },
            { "person_id", "INTEGER"               }, // may be null when only face was found but noone was assigned
            { "fingerprint_id", "INTEGER"          }, // null if fingerprint not calculated
            { "location", "CHAR(64)"               }, // format: (x),(y) (w)x(h); may be null if person was assigned, but we do not know location
            { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", ""  },
            { "FOREIGN KEY(person_id) REFERENCES " TAB_PEOPLE_NAMES "(id)", "" },
            { "FOREIGN KEY(fingerprint_id) REFERENCES " TAB_FACES_FINGERPRINTS "(id)", "" },
        });

        TableDefinition
        table_faces_fingerprints(TAB_FACES_FINGERPRINTS,
        {
            { "id", "", ColDefinition::Purpose::ID },
            { "fingerprint", "BLOB"                },
        });

        TableDefinition
        table_general_flags(TAB_GENERAL_FLAGS,
        {
            { "id", "", ColDefinition::Purpose::ID },
            { "photo_id", "INTEGER NOT NULL"       },
            { "name", "CHAR(64)"                   },
            { "value", "INTEGER"                   },
            { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", ""  },
        });

        TableDefinition
        table_photos_change_log(TAB_PHOTOS_CHANGE_LOG,
        {
            { "id", "", ColDefinition::Purpose::ID },
            { "photo_id", "INTEGER NOT NULL"       },
            { "operation", "INTEGER"               },       // add/delete/modify
            { "field", "INTEGER"                   },       // tag? flag? person?
            { "data",  QString("VARCHAR(%1)").arg(ConfigConsts::Constraints::database_tag_value_len) },
            { "date", "TIMESTAMP NOT NULL"         },
            { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", ""  },
        });

        TableDefinition
        table_phashes(TAB_PHASHES,
            {
                { "id", "", ColDefinition::Purpose::ID },
                { "photo_id", "INTEGER NOT NULL"       },
                { "hash", "BIGINT" },
                { "FOREIGN KEY(photo_id) REFERENCES " TAB_PHOTOS "(id)", ""  },
            }
        );

        //all tables
        std::map<std::string, TableDefinition> tables =
        {
            { TAB_VER,                  table_versionHistory },
            { TAB_PHOTOS,               table_photos },
            { TAB_TAGS,                 table_tags },
            { TAB_THUMBS,               table_thumbnails },
            { TAB_FLAGS,                table_flags },
            { TAB_GEOMETRY,             table_geometry },
            { TAB_GROUPS,               table_groups },
            { TAB_GROUPS_MEMBERS,       table_groups_members },
            { TAB_PEOPLE_NAMES,         table_people },
            { TAB_PEOPLE,               table_people_locations },
            { TAB_FACES_FINGERPRINTS,   table_faces_fingerprints },
            { TAB_GENERAL_FLAGS,        table_general_flags },
            { TAB_PHOTOS_CHANGE_LOG,    table_photos_change_log },
            { TAB_PHASHES,              table_phashes },
            { TAB_BLOBS,                table_blobs },
        };
}
