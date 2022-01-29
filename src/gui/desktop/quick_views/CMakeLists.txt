
find_package(Qt6 6.2 REQUIRED COMPONENTS
                 Quick
                 QuickWidgets
)

qt6_wrap_cpp(MOCED
                context_menu_manager.hpp
                media_view_ctrl.hpp
                objects_accessor.hpp
                photo_data_qml.hpp
                photos_model_controller_component.hpp
                picture_item.hpp
                qml_setup.hpp
                selection_manager_component.hpp
                thumbnail_image_provider.hpp
)

add_library(quick_views OBJECT
                ${MOCED}
                context_menu_manager.cpp
                context_menu_manager.hpp
                media_view_ctrl.cpp
                media_view_ctrl.hpp
                objects_accessor.cpp
                objects_accessor.hpp
                photo_data_qml.cpp
                photo_data_qml.hpp
                photos_model_controller_component.cpp
                photos_model_controller_component.hpp
                picture_item.cpp
                picture_item.hpp
                qml_setup.cpp
                qml_setup.hpp
                qml_utils.cpp
                qml_utils.hpp
                selection_manager_component.cpp
                selection_manager_component.hpp
                thumbnail_image_provider.cpp
                thumbnail_image_provider.hpp
)

# consider https://doc.qt.io/QtQuickCompiler/ in case of load performance problems
file(GLOB_RECURSE
    QML_RESOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml
)

qt6_add_resources(quick_views "UI"
                    PREFIX "/ui"
                    FILES ${QML_RESOURCES}
)

target_link_libraries(quick_views
                        PUBLIC
                            core
                            database
                            face_recognition
                            gui_models
                            project_utils

                            Qt::Quick
                            Qt::QuickWidgets
)

target_include_directories(quick_views
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
)
