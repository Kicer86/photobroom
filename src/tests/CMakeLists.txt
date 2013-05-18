
project(tests)

find_package(OpenLibrary REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(CppUTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src 
					${CMAKE_SOURCE_DIR}/src/analyzer
					${CMAKE_SOURCE_DIR}/src/database
                    ${CPPUTEST_INCLUDE_DIRS}
					${OPENLIBRARY_INCLUDES}
					${CMAKE_BINARY_DIR})

set(TESTS
    analyzerTests.cpp
    databaseTests.cpp
    entrydata_tests.cpp
    )

add_executable(tests RunAllTests.cpp ${TESTS})
target_link_libraries(tests analyzer database ${CPPUTEST_LIBRARIES})

turnOnCpp11(tests)
