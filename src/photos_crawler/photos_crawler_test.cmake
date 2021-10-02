
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GTest REQUIRED CONFIG)

addTestTarget(photos_crawler
                SOURCES
                    default_analyzers/file_analyzer.cpp
                    implementation/ifile_system_scanner.cpp
                    implementation/photo_crawler.cpp

                    unit_tests/analyzerTests.cpp
                    unit_tests/photo_crawler_tests.cpp
                    unit_tests/photo_crawler_builder_tests.cpp

                LIBRARIES
                    core
                    Qt::Core
                    GTest::gtest
                    GTest::gmock
                    GTest::gmock_main

                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    
                DEFINITIONS
                    PHOTOS_CRAWLER_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings 
)
