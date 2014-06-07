
project(SQL_Backends)

add_subdirectory(mysql_backend)
add_subdirectory(sqlite_backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenLibrary REQUIRED)
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Sql_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${OPENLIBRARY_INCLUDE_DIRS}
                           ${Boost_INCLUDE_DIRS})

set(SOURCES sqldbquery.cpp sql_backend.cpp table_definition.cpp tables.cpp)
set(HEADERS sqldbquery.hpp sql_backend.hpp table_definition.hpp tables.hpp)

add_library(sql_backend_base SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sql_backend_base database configuration core)
qt5_use_modules(sql_backend_base Core Sql Gui)
turnOnCpp11(sql_backend_base)
hideSymbols(sql_backend_base)
generate_export_header(sql_backend_base EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/sql_backend_base_export.h)

install(TARGETS sql_backend_base DESTINATION ${PATH_LIBS})
