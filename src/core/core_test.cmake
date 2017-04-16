
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenLibrary 2.1 REQUIRED utils)


addTestTarget(core
                SOURCES
                    unit_tests/map_iterator_tests.cpp
                    unit_tests/ptr_iterator_tests.cpp
                    unit_tests/tree_tests.cpp
                    unit_tests/ts_multi_head_queue_tests.cpp
                LIBRARIES
                    PRIVATE
                        ${GMOCK_MAIN_LIBRARY}
                        ${GMOCK_LIBRARY}
                        ${CMAKE_THREAD_LIBS_INIT}
                INCLUDES
                    SYSTEM PRIVATE
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}
                    PRIVATE
                        ${CMAKE_SOURCE_DIR}/src
                        ${OPENLIBRARY_INCLUDE_DIRS}
)
