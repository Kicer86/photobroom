
if(NOT WIN32)
    add_subdirectory(mysql_backend)
endif()

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
        group_operator.cpp
        sql_backend.cpp
        table_definition.cpp
        tables.cpp
        photo_operator.cpp
        query_structs.cpp
        sql_action_query_generator.cpp
        sql_filter_query_generator.cpp
        sql_query_executor.cpp
    )

set(HEADERS
        generic_sql_query_constructor.hpp
        group_operator.hpp
        isql_query_constructor.hpp
        isql_query_executor.hpp
        sql_backend.hpp
        table_definition.hpp
        tables.hpp
        photo_operator.hpp
        query_structs.hpp
        sql_action_query_generator.hpp
        sql_filter_query_generator.hpp
        sql_query_executor.hpp
    )

add_library(sql_backend_base SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sql_backend_base PRIVATE database core Qt5::Core Qt5::Sql Qt5::Gui)
target_include_directories(sql_backend_base PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
hideSymbols(sql_backend_base)
generate_export_header(sql_backend_base)

install(TARGETS sql_backend_base RUNTIME DESTINATION ${PATH_LIBS}
                                 LIBRARY DESTINATION ${PATH_LIBS})
