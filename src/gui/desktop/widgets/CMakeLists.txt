

include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>)
                    

set(SRCS
    editor_factory.cpp
    info_widget.cpp
    photos_data_model.cpp
    photos_widget.cpp
    staged_photos_data_model.cpp
    staged_photos_widget.cpp
    filters_editor/level_editor.cpp
    project_creator/project_creator_dialog.cpp
    tag_editor/tag_editor_widget.cpp
    tag_editor/helpers/tags_item_delegate.cpp
    tag_editor/helpers/tags_operator.cpp
    tag_editor/helpers/tags_view.cpp
    tag_editor/helpers/tags_model.cpp
    )

set(HEADERS
    editor_factory.hpp
    info_widget.hpp
    iview.hpp
    photos_data_model.hpp
    photos_widget.hpp
    staged_photos_data_model.hpp
    staged_photos_widget.hpp
    filters_editor/level_editor.hpp
    project_creator/project_creator_dialog.hpp
    tag_editor/tag_editor_widget.hpp
    tag_editor/helpers/itags_operator.hpp
    tag_editor/helpers/tags_item_delegate.hpp
    tag_editor/helpers/tags_operator.hpp
    tag_editor/helpers/tags_view.hpp
    tag_editor/helpers/tags_model.hpp
    )

#set of file to be moced
set(TO_MOC
    editor_factory.hpp
    filters_editor/level_editor.hpp
    photos_widget.hpp
    project_creator/project_creator_dialog.hpp
    staged_photos_widget.hpp
    tag_editor/tag_editor_widget.hpp
    tag_editor/helpers/tags_model.hpp
    )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_components OBJECT ${SRCS} ${MOCED} ${HEADERS})
set_target_properties(gui_components PROPERTIES POSITION_INDEPENDENT_CODE ON)
