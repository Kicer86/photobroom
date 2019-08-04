
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenLibrary 2.1 REQUIRED utils)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui  REQUIRED)


addTestTarget(core
                SOURCES
                    implementation/base_tags.cpp
                    #implementation/oriented_image.cpp
                    implementation/qmodelindex_selector.cpp
                    implementation/qmodelindex_comparator.cpp
                    implementation/tag.cpp

                    unit_tests/containers_utils_tests.cpp
                    unit_tests/map_iterator_tests.cpp
                    unit_tests/oriented_image_tests.cpp
                    unit_tests/ptr_iterator_tests.cpp
                    unit_tests/qmodelindex_comparator_tests.cpp
                    unit_tests/qmodelindex_selector_tests.cpp
                    unit_tests/status_tests.cpp
                    unit_tests/tag_info_tests.cpp
                    unit_tests/tag_name_info_tests.cpp
                    unit_tests/tag_value_tests.cpp
                LIBRARIES
                    PRIVATE
                        ${GMOCK_MAIN_LIBRARY}
                        ${GMOCK_LIBRARY}
                        ${GTEST_LIBRARY}
                        ${CMAKE_THREAD_LIBS_INIT}
                        Qt5::Core
                        Qt5::Gui
                INCLUDES
                    SYSTEM PRIVATE
                        ${GMOCK_INCLUDE_DIRS}
                        ${GTEST_INCLUDE_DIRS}
                        ${Qt5Core_INCLUDE_DIRS}
                    PRIVATE
                        ${CMAKE_SOURCE_DIR}/src
                        ${OPENLIBRARY_INCLUDE_DIRS}
                        ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_BINARY_DIR}
)
