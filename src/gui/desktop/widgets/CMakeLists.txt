
include_directories(
                    $<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:project_utils,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
)


set(SRCS
    collection_dir_scan_dialog.cpp
    color_pick_button.cpp
    info_widget.cpp
    media_preview.cpp
    multi_value_line_edit.cpp
    photo_properties.cpp
    photos_widget.cpp
    table_widget_with_limited_drop.cpp
    tasks_view_widget.cpp
    face_reviewer/face_details.cpp
    face_reviewer/face_reviewer.cpp
    face_reviewer/face_gallery.cpp
    filters_editor/level_editor.cpp
    project_creator/project_creator_dialog.cpp
    tag_editor/tag_editor_widget.cpp
    tag_editor/helpers/tags_item_delegate.cpp
    tag_editor/helpers/tags_operator.cpp
    tag_editor/helpers/tags_view.cpp
    tag_editor/helpers/tags_model.cpp
)

set(HEADERS
    collection_dir_scan_dialog.hpp
    color_pick_button.hpp
    info_widget.hpp
    media_preview.hpp
    multi_value_line_edit.hpp
    photo_properties.hpp
    photos_widget.hpp
    table_widget_with_limited_drop.hpp
    tasks_view_widget.hpp
    face_reviewer/face_details.cpp
    face_reviewer/face_reviewer.hpp
    face_reviewer/face_gallery.hpp
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
    collection_dir_scan_dialog.hpp
    filters_editor/level_editor.hpp
    photo_properties.hpp
    photos_widget.hpp
    face_reviewer/face_reviewer.hpp
    project_creator/project_creator_dialog.hpp
    tag_editor/tag_editor_widget.hpp
    tag_editor/helpers/tags_model.hpp
)

source_group(files .*)
source_group(filters_editor .*\/filters_editor\/.* )
source_group(project_creator .*\/project_creator\/.* )
source_group(tag_editor .*\/tag_editor\/.* )
source_group(generated_files \(moc.*\)|\(ui_.*\)|\(qrc_.*\) )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_widgets OBJECT ${SRCS} ${MOCED} ${HEADERS})
set_target_properties(gui_widgets PROPERTIES POSITION_INDEPENDENT_CODE ON)
