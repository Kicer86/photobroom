
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(enumFlex enum.l ${CMAKE_CURRENT_BINARY_DIR}/enum.yy.cc
    COMPILE_FLAGS "-CF --header-file=${CMAKE_CURRENT_BINARY_DIR}/enum.yy.hh")

bison_target(enumBison enum.y ${CMAKE_CURRENT_BINARY_DIR}/enum.tab.cc)
add_flex_bison_dependency(enumFlex enumBison)

add_executable(enum_to_string
                ${FLEX_enumFlex_OUTPUTS}
                ${BISON_enumBison_OUTPUTS}
                main.cpp
)

target_include_directories(enum_to_string
                            PRIVATE
                                ${CMAKE_CURRENT_BINARY_DIR}
)


if(BUILD_TESTS)
    include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

    addTestTarget(enum_to_string
                    SOURCES
                        ${FLEX_enumFlex_OUTPUTS}
                        ${BISON_enumBison_OUTPUTS}
                        unit_test.cpp
                    LIBRARIES
                        PRIVATE
                            ${GMOCK_MAIN_LIBRARY}
                            ${GMOCK_LIBRARY}
                            ${GTEST_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT}
                    INCLUDES
                        SYSTEM PRIVATE
                            ${GMOCK_INCLUDE_DIRS}
                            ${GTEST_INCLUDE_DIRS}
                        PRIVATE
                            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
