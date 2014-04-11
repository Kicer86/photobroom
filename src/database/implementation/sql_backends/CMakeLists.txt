
project(SQL_Backends)

add_subdirectory(mysql_backend)
add_subdirectory(sqlite_backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(OpenLibrary REQUIRED)
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(SOURCES backend_builder.cpp sqldbquery.cpp sql_backend.cpp table_definition.cpp)
set(HEADERS backend_builder.hpp sqldbquery.hpp sql_backend.hpp table_definition.hpp)

add_library(sql_backend_base SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sql_backend_base configuration core)
qt5_use_modules(sql_backend_base Core Sql)
turnOnCpp11(sql_backend_base)
hideSymbols(sql_backend_base)
generate_export_header(sql_backend_base EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/sql_backend_base_export.h)

install(TARGETS sql_backend_base RUNTIME LIBRARY DESTINATION lib/broom/)
