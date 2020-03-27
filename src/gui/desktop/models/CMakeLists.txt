
set(SRC
    aphoto_info_model.cpp
    db_data_model.cpp
    flat_model.cpp
    photo_properties.cpp
    model_helpers/idx_data.cpp
    model_helpers/idx_data_manager.cpp
    model_helpers/idxdata_deepfetcher.cpp
    model_helpers/photos_matcher.cpp
)

set(HEADERS
    aphoto_info_model.hpp
    db_data_model.hpp
    flat_model.hpp
    model_types.hpp
    photo_properties.hpp
    model_helpers/idx_data.hpp
    model_helpers/idx_data_manager.hpp
    model_helpers/idxdata_deepfetcher.hpp
    model_helpers/photos_matcher.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

add_library(gui_models OBJECT ${SRC} ${HEADERS})
set_target_properties(gui_models PROPERTIES AUTOMOC ON)
set_target_properties(gui_models PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_models
                        PUBLIC
                            database
                        PRIVATE
                            core
)

target_include_directories(gui_models
                            INTERFACE
                                ${CMAKE_CURRENT_SOURCE_DIR}/..          #require "model/" from clients
                            PRIVATE
                                ${CMAKE_CURRENT_SOURCE_DIR}
)
