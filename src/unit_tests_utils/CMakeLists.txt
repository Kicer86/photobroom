
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

stringify_file(${CMAKE_CURRENT_BINARY_DIR}/db_for_series_detection.json.hpp ${CMAKE_CURRENT_SOURCE_DIR}/sample_dbs/db_for_series_detection.json "const char* db" "SeriesDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/phash_db.json.hpp ${CMAKE_CURRENT_SOURCE_DIR}/sample_dbs/phash_db.json "const char* db" "PHashDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/rich_db.json.hpp ${CMAKE_CURRENT_SOURCE_DIR}/sample_dbs/rich_db.json "const char* db1" "RichDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/sample_db.json.hpp  ${CMAKE_CURRENT_SOURCE_DIR}/sample_dbs/sample_db.json  "const char* db1" "SampleDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/sample_db2.json.hpp ${CMAKE_CURRENT_SOURCE_DIR}/sample_dbs/sample_db2.json "const char* db2" "SampleDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/sample_db_with_groups.json.hpp ${CMAKE_CURRENT_SOURCE_DIR}/sample_dbs/sample_db_with_groups.json "const char* db" "GroupsDB")

add_library(sample_dbs INTERFACE)

target_sources(sample_dbs
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/db_for_series_detection.json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/phash_db.json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/rich_db.json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/sample_db.json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/sample_db2.json.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/sample_db_with_groups.json.hpp
)

target_include_directories(sample_dbs
    INTERFACE
        ${PROJECT_BINARY_DIR}/src
)
