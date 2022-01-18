
set(SRC
    actions_model.cpp
    actions_model.hpp
    aphoto_info_model.cpp
    aphoto_info_model.hpp
    flat_model.cpp
    flat_model.hpp
    model_types.hpp
    notifications_model.cpp
    notifications_model.hpp
    roles_expansion.hpp
    series_model.cpp
    series_model.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

add_library(gui_models OBJECT ${SRC})
set_target_properties(gui_models PROPERTIES AUTOMOC ON)
target_link_libraries(gui_models
                        PUBLIC
                            database
                        PRIVATE
                            core
                            project_utils
)

target_include_directories(gui_models
                            INTERFACE
                                ${CMAKE_CURRENT_SOURCE_DIR}/..          #require "model/" from clients
                            PRIVATE
                                ${CMAKE_CURRENT_SOURCE_DIR}
)
