
find_package(Qt5 REQUIRED COMPONENTS
                 Quick
                 QuickWidgets
)

# consider https://doc.qt.io/QtQuickCompiler/ in case of load performance problems
qt5_add_resources(QML_RESOURCES
                    resources.qrc
)

qt5_wrap_cpp(MOCED
                photo_item.hpp
                photos_model_controller_component.hpp
                qml_setup.hpp
                selection_manager_component.hpp
)


add_library(quick_views OBJECT
                ${QML_RESOURCES}
                ${MOCED}
                photo_item.cpp
                photo_item.hpp
                photos_model_controller_component.cpp
                photos_model_controller_component.hpp
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
                            gui_models

                            Qt::Quick
                            Qt::QuickWidgets
)

target_include_directories(quick_views
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
)

generate_export_header(quick_views)
