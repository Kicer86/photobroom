
project(tests)


set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(CppUTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/analyzer ${CPPUTEST_INCLUDE_DIRS})

set(TESTS
    analyzerTests.cpp
    entrydata_tests.cpp
    )

add_executable(tests RunAllTests.cpp ${TESTS})
target_link_libraries(tests analyzer ${CPPUTEST_LIBRARIES})

turnOnCpp11(tests)
