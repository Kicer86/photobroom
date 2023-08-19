
set(SRC
    actions_model.cpp
    actions_model.hpp
    aheavy_list_model.cpp
    aphoto_data_model.cpp
    aphoto_data_model.hpp
    duplicates_model.cpp
    duplicates_model.hpp
    faces_model.cpp
    faces_model.hpp
    flat_model.cpp
    flat_model.hpp
    itags_operator.hpp
    notifications_model.cpp
    notifications_model.hpp
    photo_properties_model.cpp
    photo_properties_model.hpp
    roles_expansion.hpp
    series_model.cpp
    series_model.hpp
    tags_model.cpp
    tags_model.hpp
    tags_operator.cpp
    tags_operator.hpp
    tasks_model.cpp
    tasks_model.hpp
)

add_library(gui_models OBJECT ${SRC})
set_target_properties(gui_models PROPERTIES AUTOMOC ON)
target_link_libraries(gui_models
                        PUBLIC
                            database
                        PRIVATE
                            core
                            face_recognition
                            project_utils
                            system
)

target_include_directories(gui_models
                            PUBLIC
                                ${CMAKE_CURRENT_SOURCE_DIR}/..          # require "model/" from clients
                                ${CMAKE_CURRENT_SOURCE_DIR}/../..       # gui's top directory
                            PRIVATE
                                ${CMAKE_CURRENT_SOURCE_DIR}
)
