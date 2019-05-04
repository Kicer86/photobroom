
set(SRC
    aphoto_info_model.cpp
    db_data_model.cpp
    model_helpers/idx_data.cpp
    model_helpers/idx_data_manager.cpp
    model_helpers/idxdata_deepfetcher.cpp
    model_helpers/photos_matcher.cpp
    )

set(HEADERS
    aphoto_info_model.hpp
    db_data_model.hpp
    model_types.hpp
    model_helpers/idx_data.hpp
    model_helpers/idx_data_manager.hpp
    model_helpers/idxdata_deepfetcher.hpp
    model_helpers/photos_matcher.hpp
    )

set(TO_MOC
    model_helpers/idx_data_manager.hpp
    model_helpers/idxdata_deepfetcher.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_models OBJECT ${SRC} ${MOCED} ${HEADERS})
set_target_properties(gui_models PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_models
                        PRIVATE
                            core
                            database
)

target_include_directories(gui_models
                            PRIVATE
                                ${CMAKE_CURRENT_SOURCE_DIR}
)
