
include(${CMAKE_SOURCE_DIR}/cmake_private/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenLibrary 2.1 REQUIRED utils)
find_package(Qt5Core REQUIRED)


addTestTarget(core
                SOURCES
                    unit_tests/map_iterator_tests.cpp
                    unit_tests/ptr_iterator_tests.cpp
                    unit_tests/qmodelindex_comparator_tests.cpp
                    unit_tests/qmodelindex_selector_tests.cpp
                    unit_tests/tree_tests.cpp
                    unit_tests/ts_multi_queue_tests.cpp
                LIBRARIES
                    PRIVATE
                        ${GMOCK_MAIN_LIBRARY}
                        ${GMOCK_LIBRARY}
                        ${GTEST_LIBRARY}
                        ${CMAKE_THREAD_LIBS_INIT}
                        Qt5::Core
                        core
                INCLUDES
                    SYSTEM PRIVATE
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}
                        ${Qt5Core_INCLUDE_DIRS}
                    PRIVATE
                        ${CMAKE_SOURCE_DIR}/src
                        ${OPENLIBRARY_INCLUDE_DIRS}
)
