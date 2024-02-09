
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GTest REQUIRED CONFIG)

add_executable(photos_crawler_ut
    default_analyzers/file_analyzer.cpp
    implementation/ifile_system_scanner.cpp
    implementation/photo_crawler.cpp

    unit_tests/analyzerTests.cpp
    unit_tests/photo_crawler_tests.cpp
)

target_link_libraries(photos_crawler_ut
    PRIVATE
        core
        Qt::Core
        GTest::gtest
        GTest::gmock
        GTest::gmock_main
)

target_include_directories(photos_crawler_ut
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_definitions(photos_crawler_ut
    PRIVATE
        PHOTOS_CRAWLER_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)

add_test(
    NAME photos_crawler
    COMMAND photos_crawler_ut
)

register_unit_test(photos_crawler photos_crawler_ut)
