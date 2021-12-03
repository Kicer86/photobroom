
find_package(Qt6 6.2 REQUIRED COMPONENTS
                 Quick
                 QuickWidgets
)

# consider https://doc.qt.io/QtQuickCompiler/ in case of load performance problems
qt6_add_resources(QML_RESOURCES
                    resources.qrc
)

qt6_wrap_cpp(MOCED
                media_renderers/gif_renderer.hpp
                amedia_item.hpp
                media_view_ctrl.hpp
                media_view_item.hpp
                objects_accessor.hpp
                photo_data_qml.hpp
                photo_item.hpp
                photos_model_controller_component.hpp
                picture_item.hpp
                qml_setup.hpp
                selection_manager_component.hpp
)

add_library(quick_views OBJECT
                ${QML_RESOURCES}
                ${MOCED}
                media_renderers/gif_renderer.cpp
                media_renderers/gif_renderer.hpp
                amedia_item.cpp
                amedia_item.hpp
                media_view_ctrl.cpp
                media_view_ctrl.hpp
                media_view_item.cpp
                media_view_item.hpp
                objects_accessor.cpp
                objects_accessor.hpp
                photo_data_qml.cpp
                photo_data_qml.hpp
                photo_item.cpp
                photo_item.hpp
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
)


target_link_libraries(quick_views
                        PUBLIC
                            core
                            database
                            gui_models

                            Qt::Quick
                            Qt::QuickWidgets
)

target_include_directories(quick_views
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
)
