
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Sql REQUIRED)

add_definitions(-DSTATIC_PLUGINS)  # build in all plugins

addTestTarget(database_integration
                SOURCES

                    # engines
                    backends/sql_backends/sqlite_backend/backend.cpp
                    backends/sql_backends/mysql_backend/backend.cpp
                    backends/sql_backends/mysql_backend/mysql_server.cpp

                    # other sql stuff
                    backends/sql_backends/generic_sql_query_constructor.cpp
                    backends/sql_backends/group_operator.cpp
                    backends/sql_backends/people_information_accessor.cpp
                    backends/sql_backends/photo_change_log_operator.cpp
                    backends/sql_backends/photo_operator.cpp
                    backends/sql_backends/sql_filter_query_generator.cpp
                    backends/sql_backends/sql_query_executor.cpp
                    backends/sql_backends/query_structs.cpp
                    backends/sql_backends/sql_backend.cpp
                    backends/sql_backends/table_definition.cpp
                    backends/sql_backends/tables.cpp
                    backends/sql_backends/transaction.cpp

                    # sql tests:
                    integration_tests/common.hpp
                    integration_tests/general_flags_tests.cpp
                    integration_tests/groups_tests.cpp
                    integration_tests/people_tests.cpp
                    integration_tests/photos_change_log_tests.cpp

                    # main()
                    integration_tests/main.cpp

                LIBRARIES
                    core
                    database
                    plugins
                    system
                    Qt5::Core
                    Qt5::Gui
                    Qt5::Sql
                    ${GMOCK_LIBRARY}
                    ${GTEST_LIBRARY}
                    ${CMAKE_THREAD_LIBS_INIT}


                SYSTEM_INCLUDES
                    ${GMOCK_INCLUDE_DIRS}
                    ${GTEST_INCLUDE_DIRS}

                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends
                    ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends/sqlite_backend
                    ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends/mysql_backend

                DEFINITIONS
                    STATIC_PLUGINS
)
