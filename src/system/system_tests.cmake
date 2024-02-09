
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GTest REQUIRED CONFIG)
find_package(Qt6 REQUIRED COMPONENTS Core)


add_executable(system_ut
    unit_tests/common_path.cpp
    implementation/filesystem.cpp
)

target_link_libraries(system_ut
    PRIVATE
        GTest::gtest
        GTest::gmock
        GTest::gmock_main
        Qt::Core
)

target_include_directories(system_ut
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}
)

target_compile_definitions(system_ut
    PRIVATE
        SYSTEM_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)

add_test(
    NAME system
    COMMAND system_ut
)

register_unit_test(system system_ut)
