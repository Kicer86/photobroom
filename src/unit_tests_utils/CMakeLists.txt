
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

stringify_file(${CMAKE_CURRENT_BINARY_DIR}/sample_db.json.hpp  sample_dbs/sample_db.json  "const char* db1" "SampleDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/sample_db2.json.hpp sample_dbs/sample_db2.json "const char* db2" "SampleDB")
stringify_file(${CMAKE_CURRENT_BINARY_DIR}/db_for_series_detection.json.hpp sample_dbs/db_for_series_detection.json "const char* db" "SeriesDB")

add_library(sample_dbs INTERFACE)

target_include_directories(sample_dbs
    INTERFACE
        ${PROJECT_BINARY_DIR}/src
)
