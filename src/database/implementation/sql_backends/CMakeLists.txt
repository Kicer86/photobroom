
project(SQL_Backends)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(OpenLibrary REQUIRED)
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(SOURCES sqldbquery.cpp sql_backend.cpp table_definition.cpp)
set(HEADERS sqldbquery.hpp sql_backend.hpp table_definition.hpp)

add_library(sql_backend_base SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sql_backend_base configuration core)
qt5_use_modules(sql_backend_base Core Sql)
turnOnCpp11(sql_backend_base)
hideSymbols(sql_backend_base)
generate_export_header(sql_backend_base EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/sql_backend_base_export.h)


set(BACKEND mysql)

#determine which backend should be used
if(BACKEND STREQUAL "mysql")

    add_subdirectory(mysql_backend)
    #target_link_libraries(sql_backend_base database_mysql_backend)

endif(BACKEND STREQUAL "mysql")
