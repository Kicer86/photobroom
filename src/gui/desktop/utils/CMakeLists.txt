
add_subdirectory(animated_webp)

find_package(Qt6 REQUIRED COMPONENTS Svg Quick QuickWidgets)
find_package(OpenCV REQUIRED)

set(UTILS_SOURCES
    grouppers/animation_generator.cpp
    grouppers/animation_generator.hpp
    grouppers/collage_generator.cpp
    grouppers/collage_generator.hpp
    grouppers/generator_utils.cpp
    grouppers/generator_utils.hpp
    grouppers/hdr_generator.cpp
    grouppers/hdr_generator.hpp
    collection_scanner.cpp
    collection_scanner.hpp
    config_tools.cpp
    config_tools.hpp
    features_manager.cpp
    features_manager.hpp
    features_observer.cpp
    features_observer.hpp
    groups_manager.cpp
    groups_manager.hpp
    ithumbnails_cache.hpp
    ithumbnails_manager.hpp
    model_index_utils.cpp
    model_index_utils.hpp
    painter_helpers.cpp
    painter_helpers.hpp
    people_list_model.cpp
    people_list_model.hpp
    people_manipulator.cpp
    people_manipulator.hpp
    photos_collector.cpp
    photos_collector.hpp
    qml_setup.cpp
    qml_setup.hpp
    qml_utils.cpp
    qml_utils.hpp
    signal_filter.cpp
    signal_filter.hpp
    svg_utils.cpp
    svg_utils.hpp
    tag_value_model.cpp
    tag_value_model.hpp
    thumbnails_cache.hpp
    thumbnails_cache.cpp
    thumbnail_manager.hpp
    thumbnail_manager.cpp
    variant_display.cpp
    variant_display.hpp
)

add_library(gui_utils OBJECT ${UTILS_SOURCES})
set_target_properties(gui_utils PROPERTIES AUTOMOC TRUE)
target_link_libraries(gui_utils
                        PRIVATE
                            core
                            database
                            face_recognition
                            photos_crawler
                            system
                            project_utils

                            AnimatedWebP
                            Qt::Svg
                            Qt::Quick
                            Qt::QuickWidgets
                            opencv_imgcodecs
)

target_include_directories(gui_utils
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
)

