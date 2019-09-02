
set(UTILS_SOURCES
    config_tools.cpp
    groups_manager.cpp
    model_index_utils.cpp
    painter_helpers.cpp
    people_list_model.cpp
    people_operator.cpp
    photos_collector.cpp
    selection_extractor.cpp
    signal_filter.cpp
    tag_value_model.cpp
    thumbnail_generator.cpp
    variant_display.cpp
    grouppers/animation_generator.cpp
    grouppers/generator_utils.cpp
    grouppers/hdr_generator.cpp
)

set(UTILS_HEADERS
    config_tools.hpp
    groups_manager.hpp
    ithumbnail_generator.hpp
    model_index_utils.hpp
    painter_helpers.hpp
    people_list_model.hpp
    people_operator.hpp
    people_operator_p.hpp
    photos_collector.hpp
    selection_extractor.hpp
    signal_filter.hpp
    tag_value_model.hpp
    thumbnail_generator.hpp
    variant_display.hpp
    grouppers/animation_generator.hpp
    grouppers/generator_utils.hpp
    grouppers/hdr_generator.hpp
)


set(UTILS_FILES_TO_MOC
    people_operator.hpp
    people_operator_p.hpp
    photos_collector.hpp
    selection_extractor.hpp
    signal_filter.hpp
    grouppers/animation_generator.hpp
    grouppers/generator_utils.hpp
    grouppers/hdr_generator.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(UTILS_WRAPPED_CPP ${UTILS_FILES_TO_MOC})

add_library(gui_utils OBJECT ${UTILS_SOURCES} ${UTILS_WRAPPED_CPP} ${UTILS_HEADERS})
set_target_properties(gui_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_utils
                        PRIVATE
                            core
                            database
                            face_recognition
                            photos_crawler
                            system
                            project_utils
)

target_include_directories(gui_utils
                            SYSTEM PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
)

