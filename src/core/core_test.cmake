
include(${CMAKE_SOURCE_DIR}/cmake/functions.cmake)

find_package(GTest REQUIRED CONFIG)
find_package(Qt6     REQUIRED COMPONENTS Core Gui)
find_package(Qt6Test REQUIRED)


addTestTarget(core
                SOURCES
                    implementation/base_tags.cpp
                    #implementation/oriented_image.cpp
                    implementation/exiftool_video_details_reader.cpp
                    implementation/model_compositor.cpp
                    implementation/qmodelindex_selector.cpp
                    implementation/qmodelindex_comparator.cpp
                    implementation/tag.cpp
                    implementation/task_executor_utils.cpp
                    imodel_compositor_data_source.hpp

                    unit_tests/containers_utils_tests.cpp
                    unit_tests/exiftool_video_details_reader_tests.cpp
                    unit_tests/function_wrappers_tests.cpp
                    unit_tests/lazy_ptr_tests.cpp
                    unit_tests/model_compositor_tests.cpp
                    #unit_tests/oriented_image_tests.cpp
                    unit_tests/qmodelindex_comparator_tests.cpp
                    unit_tests/qmodelindex_selector_tests.cpp
                    unit_tests/status_tests.cpp
                    unit_tests/tag_name_info_tests.cpp
                    unit_tests/tag_value_tests.cpp
                LIBRARIES
                    GTest::gtest
                    GTest::gmock
                    GTest::gmock_main
                    Qt::Core
                    Qt::Gui
                    Qt::Test

                INCLUDES
                    ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}

                DEFINITIONS
                    CORE_STATIC_DEFINE                  # disable visibility mechanisms to prevent inconsistent dll linkage warnings
)
