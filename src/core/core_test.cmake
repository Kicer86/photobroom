
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenLibrary 2.1 REQUIRED utils)

set(SRC
        unit_tests/tree_tests.cpp
        unit_tests/ts_multi_head_queue_tests.cpp
    )

add_executable(core_tests ${SRC})

target_link_libraries(core_tests
                        PRIVATE
                            ${GMOCK_MAIN_LIBRARY}
                            ${GMOCK_LIBRARY}
                            ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(core_tests
                            SYSTEM PRIVATE
                                ${GMOCK_INCLUDE_DIRS}
                                ${GTEST_INCLUDE_DIRS}
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src
                                ${OPENLIBRARY_INCLUDE_DIRS}
)


enableCodeCoverage(core_tests)

add_test(core core_tests)
