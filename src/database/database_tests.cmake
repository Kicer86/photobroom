
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Qt6    REQUIRED COMPONENTS Sql Test)
find_package(OpenCV REQUIRED)

add_executable(database_ut
    backends/memory_backend/memory_backend.cpp
    backends/sql_backends/generic_sql_query_constructor.cpp
    backends/sql_backends/sql_filter_query_generator.cpp
    backends/sql_backends/query_structs.cpp
    backends/sql_backends/utils.cpp
    database_tools/implementation/json_to_backend.cpp
    database_tools/implementation/series_detector.cpp
    database_tools/implementation/tag_info_collector.cpp
    implementation/apeople_information_accessor.cpp
    implementation/aphoto_change_log_operator.cpp
    implementation/filter.cpp
    implementation/notifications_accumulator.cpp
    implementation/person_data.cpp
    implementation/photo_data.cpp
    implementation/photo_utils.cpp
    notifications_accumulator.hpp
    ibackend.hpp

    # tests:
    unit_tests/data_delta_tests.cpp
    unit_tests/db_error_tests.cpp
    unit_tests/explicit_photo_delta_tests.cpp
    unit_tests/generic_sql_query_constructor_tests.cpp
    unit_tests/json_to_backend_tests.cpp
    unit_tests/memory_backend_tests.cpp
    unit_tests/notifications_accumulator_tests.cpp
    unit_tests/sql_filter_query_generator_tests.cpp
    unit_tests/series_detector_tests.cpp
    unit_tests/tag_info_collector_tests.cpp

    # main()
    unit_tests/main.cpp
)

target_link_libraries(database_ut
    PRIVATE
        core
        database
        sample_dbs
        Qt::Core
        Qt::Gui
        Qt::Sql
        Qt::Test
        GTest::gtest
        GTest::gmock
        magic_enum::magic_enum
        opencv_img_hash
)

target_include_directories(database_ut
    PRIVATE
        backends/sql_backends
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/backends/sql_backends
        ${CMAKE_CURRENT_BINARY_DIR}/backends/memory_backend
        ${OpenCV_INCLUDE_DIRS}
)

target_compile_definitions(database_ut
    PRIVATE
        STATIC_PLUGINS
        DATABASE_MEMORY_BACKEND_STATIC_DEFINE   # disable visibility mechanisms to prevent inconsistent dll linkage warnings
        DATABASE_MYSQL_BACKEND_STATIC_DEFINE    # disable visibility mechanisms to prevent inconsistent dll linkage warnings
        DATABASE_SQLITE_BACKEND_STATIC_DEFINE   # disable visibility mechanisms to prevent inconsistent dll linkage warnings
        DATABASE_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
        SQL_BACKEND_BASE_STATIC_DEFINE          # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)

add_test(
    NAME database
    COMMAND database_ut
)

register_unit_test(database database_ut)
