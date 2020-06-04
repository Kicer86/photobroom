
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

stringify_file(${CMAKE_CURRENT_BINARY_DIR}/sample_db.json.hpp sample_dbs/sample_db.json "const char* db1" "SampleDB")

add_library(sample_dbs INTERFACE)

target_include_directories(sample_dbs
    INTERFACE
        ${PROJECT_BINARY_DIR}/src
)
