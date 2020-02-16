
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

flex_target(enumFlex ${CMAKE_CURRENT_SOURCE_DIR}/enum.l ${CMAKE_CURRENT_BINARY_DIR}/enum.yy.cc
    COMPILE_FLAGS "-CF --header-file=${CMAKE_CURRENT_BINARY_DIR}/enum.yy.hh")

bison_target(enumBison ${CMAKE_CURRENT_SOURCE_DIR}/enum.y ${CMAKE_CURRENT_BINARY_DIR}/enum.tab.cc)
add_flex_bison_dependency(enumFlex enumBison)

add_executable(enum_to_string
                ${FLEX_enumFlex_OUTPUTS}
                ${BISON_enumBison_OUTPUTS}
                main.cpp
                logic.cpp
)

if(WIN32)
    target_compile_definitions(enum_to_string PRIVATE YY_NO_UNISTD_H)
endif()


if(BUILD_TESTING)
    find_package(Threads REQUIRED)
    find_package(GMock REQUIRED)
    find_package(GTest REQUIRED)

    include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

    addTestTarget(enum_to_string
                    SOURCES
                        ${FLEX_enumFlex_OUTPUTS}
                        ${BISON_enumBison_OUTPUTS}
                        unit_test.cpp
                        logic.cpp
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
    )
endif()
