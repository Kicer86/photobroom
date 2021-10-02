
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GTest REQUIRED CONFIG)
find_package(Qt6 REQUIRED COMPONENTS Core)

addTestTarget(system
                SOURCES
                    unit_tests/common_path.cpp
                    implementation/filesystem.cpp
                LIBRARIES
                    GTest::gtest
                    GTest::gmock
                    GTest::gmock_main
                    Qt::Core

                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
                    
                DEFINITIONS
                    SYSTEM_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings 
)
