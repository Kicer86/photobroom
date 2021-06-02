
find_package(Qt6 REQUIRED COMPONENTS Svg)

set(UTILS_SOURCES
    grouppers/animation_generator.cpp
    grouppers/animation_generator.hpp
    grouppers/collage_generator.cpp
    grouppers/collage_generator.hpp
    grouppers/generator_utils.cpp
    grouppers/generator_utils.hpp
    grouppers/hdr_generator.cpp
    grouppers/hdr_generator.hpp
    config_tools.cpp
    config_tools.hpp
    features_manager.cpp
    features_manager.hpp
    groups_manager.cpp
    groups_manager.hpp
    inotifications.hpp
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
    selection_to_photoid_translator.cpp
    selection_to_photoid_translator.hpp
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

source_group(files REGULAR_EXPRESSION .*gui.* )

add_library(gui_utils OBJECT ${UTILS_SOURCES})
set_target_properties(gui_utils PROPERTIES AUTOMOC TRUE)
set_target_properties(gui_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_utils
                        PRIVATE
                            core
                            database
                            face_recognition
                            photos_crawler
                            system
                            project_utils

                            Qt::Svg
)

target_include_directories(gui_utils
                            SYSTEM PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
)

