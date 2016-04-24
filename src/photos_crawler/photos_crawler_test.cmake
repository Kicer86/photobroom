
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

include_directories(SYSTEM ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

set(SRC
        default_analyzers/ext_defaultanalyzer.cpp
        default_analyzers/file_analyzer.cpp
        implementation/ianalyzer.cpp
        implementation/ifile_system_scanner.cpp
        implementation/photo_crawler.cpp

        unit_tests/analyzerTests.cpp
        unit_tests/photo_crawler_tests.cpp
        unit_tests/photo_crawler_builder_tests.cpp
    )

add_executable(photos_crawler_tests ${SRC})

target_link_libraries(photos_crawler_tests PRIVATE Qt5::Core ${GMOCK_MAIN_LIBRARY} ${GMOCK_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
enableCodeCoverage(photos_crawler_tests)

add_test(photos_crawler photos_crawler_tests)
