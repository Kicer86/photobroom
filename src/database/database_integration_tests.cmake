
include(${CMAKE_SOURCE_DIR}/cmake_private/functions.cmake)

find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Sql REQUIRED)


set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends/sqlite_backend/moc_backend.cpp PROPERTIES GENERATED TRUE)


addTestTarget(database_integration
                SOURCES
                    backends/sql_backends/sqlite_backend/backend.cpp
                    backends/sql_backends/generic_sql_query_constructor.cpp
                    backends/sql_backends/sql_action_query_generator.cpp
                    backends/sql_backends/sql_filter_query_generator.cpp
                    backends/sql_backends/sql_query_executor.cpp
                    backends/sql_backends/query_structs.cpp
                    backends/sql_backends/sql_backend.cpp
                    backends/sql_backends/table_definition.cpp
                    backends/sql_backends/tables.cpp
                    $<TARGET_OBJECTS:plugins>
                    ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends/sqlite_backend/moc_backend.cpp

                    # sql tests:
                    integration_tests/sql_backend_tests.cpp

                    # main()
                    integration_tests/main.cpp

                LIBRARIES
                    PRIVATE
                        core
                        database
                        system
                        Qt5::Core
                        Qt5::Gui
                        Qt5::Sql
                        ${GMOCK_LIBRARY}
                        ${GTEST_LIBRARY}
                        ${CMAKE_THREAD_LIBS_INIT}

                INCLUDES
                    SYSTEM PRIVATE
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}

                    PRIVATE
                        ${CMAKE_SOURCE_DIR}/src
                        ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends
                        ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends/sqlite_backend
)
