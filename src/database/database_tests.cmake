
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
find_package(Qt5Sql REQUIRED)

include_directories(SYSTEM
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}
                        ${Qt5Sql_INCLUDE_DIRS}
)

include_directories(
    backends/sql_backends
    ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends
)


set(SRC
        backends/sql_backends/generic_sql_query_constructor.cpp
        backends/sql_backends/sql_action_query_generator.cpp
        backends/sql_backends/sql_filter_query_generator.cpp
        backends/sql_backends/query_structs.cpp
        implementation/action.cpp
        implementation/filter.cpp
        implementation/photo_data.cpp
        implementation/photo_info.cpp
        implementation/photo_types.cpp

        # sql tests:
        unit_tests/generic_sql_query_constructor_tests.cpp
        unit_tests/sql_filter_query_generator_tests.cpp
        unit_tests/sql_action_query_generator_tests.cpp

        # tools tests:

        unit_tests/tag_info_collector_tests.cpp

        # main()
        unit_tests/main.cpp
    )

add_executable(database_tests ${SRC})

target_link_libraries(database_tests
                        PRIVATE
                            core
                            database
                            Qt5::Core
                            Qt5::Gui
                            Qt5::Sql
                            ${GMOCK_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT}
)

enableCodeCoverage(database_tests)

add_test(database database_tests)
