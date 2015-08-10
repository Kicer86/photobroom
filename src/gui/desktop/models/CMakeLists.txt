
include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>)

set(SRC
    db_data_model.cpp
    image_list_model.cpp
    photos_data_model.cpp
    staged_photos_data_model.cpp
    model_helpers/idx_data.cpp
    model_helpers/idx_data_manager.cpp
    model_helpers/idxdata_deepfetcher.cpp
    model_helpers/photos_matcher.cpp
    )

set(HEADERS
    db_data_model.hpp
    image_list_model.hpp
    image_list_model_p.hpp
    photos_data_model.hpp
    staged_photos_data_model.hpp
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
