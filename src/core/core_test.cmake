
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)

include_directories(SYSTEM ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS})

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


enableCodeCoverage(core_tests)

add_test(core core_tests)
