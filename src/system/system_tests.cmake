
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GTest REQUIRED CONFIG)
find_package(Qt5Core 5.4 REQUIRED)

addTestTarget(system
                SOURCES
                    unit_tests/common_path.cpp
                    implementation/filesystem.cpp
                LIBRARIES
                    GTest::gtest
                    GTest::gmock
                    GTest::gmock_main
                    Qt5::Core

                SYSTEM_INCLUDES
                    ${Qt5Core_INCLUDE_DIRS}

                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
)
