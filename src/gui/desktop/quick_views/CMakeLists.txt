
find_package(Qt6 6.2 REQUIRED COMPONENTS
                 Quick
                 QuickWidgets
)

# consider https://doc.qt.io/QtQuickCompiler/ in case of load performance problems
qt6_add_resources(QML_RESOURCES
                    resources.qrc
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
                ${QML_RESOURCES}
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

target_link_libraries(quick_views
                        PUBLIC
                            core
                            database
                            gui_models
                            project_utils

                            Qt::Quick
                            Qt::QuickWidgets
)

target_include_directories(quick_views
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
)
