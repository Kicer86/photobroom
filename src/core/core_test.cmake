
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(Threads REQUIRED)
find_package(GMock REQUIRED)
find_package(GTest REQUIRED)
find_package(OpenLibrary 2.1 REQUIRED utils)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui  REQUIRED)
find_package(Qt5Test REQUIRED)


addTestTarget(core
                SOURCES
                    implementation/base_tags.cpp
                    #implementation/oriented_image.cpp
                    implementation/model_compositor.cpp
                    implementation/qmodelindex_selector.cpp
                    implementation/qmodelindex_comparator.cpp
                    implementation/tag.cpp
                    implementation/thumbnail_manager.cpp
                    implementation/thumbnails_cache.cpp
                    implementation/task_executor_utils.cpp
                    imodel_compositor_data_source.hpp

                    unit_tests/containers_utils_tests.cpp
                    unit_tests/lazy_ptr_tests.cpp
                    unit_tests/map_iterator_tests.cpp
                    unit_tests/model_compositor_tests.cpp
                    #unit_tests/oriented_image_tests.cpp
                    unit_tests/ptr_iterator_tests.cpp
                    unit_tests/qmodelindex_comparator_tests.cpp
                    unit_tests/qmodelindex_selector_tests.cpp
                    unit_tests/status_tests.cpp
                    unit_tests/tag_info_tests.cpp
                    unit_tests/tag_name_info_tests.cpp
                    unit_tests/tag_value_tests.cpp
                    unit_tests/thumbnails_manager_tests.cpp
                    unit_tests/thumbnails_cache_tests.cpp
                LIBRARIES
                    ${GMOCK_MAIN_LIBRARY}
                    ${GMOCK_LIBRARY}
                    ${GTEST_LIBRARY}
                    ${CMAKE_THREAD_LIBS_INIT}
                    Qt5::Core
                    Qt5::Gui
                    Qt5::Test
                SYSTEM_INCLUDES
                    ${GMOCK_INCLUDE_DIRS}
                    ${GTEST_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${OPENLIBRARY_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
)
