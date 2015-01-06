
add_subdirectory(mysql_backend)
add_subdirectory(sqlite_backend)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenLibrary REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Sql_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${OPENLIBRARY_INCLUDE_DIRS})

set(SOURCES
        generic_sql_query_constructor.cpp
        #sql_db_query.cpp                         deprecated
        sql_backend.cpp
        table_definition.cpp
        tables.cpp
        query_structs.cpp
        sql_query_constructor.cpp
        sql_select_query_generator.cpp
    )

set(HEADERS
        generic_sql_query_constructor.hpp
        sql_db_query.hpp
        sql_backend.hpp
        table_definition.hpp
        tables.hpp
        query_structs.hpp
        sql_query_constructor.hpp
        sql_select_query_generator.hpp
    )

add_library(sql_backend_base SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sql_backend_base PRIVATE database configuration core Qt5::Core Qt5::Sql Qt5::Gui)
turnOnCpp11(sql_backend_base)
hideSymbols(sql_backend_base)
generate_export_header(sql_backend_base EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/sql_backend_base_export.h)

install(TARGETS sql_backend_base RUNTIME DESTINATION ${PATH_LIBS}
                                 LIBRARY DESTINATION ${PATH_LIBS})
