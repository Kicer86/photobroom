
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Quick
    QuickWidgets
)

set(CMAKE_AUTOMOC ON)

set(QUICK_ITEMS_CPP_SOURCES
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
    selection_manager_component.cpp
    selection_manager_component.hpp
    thumbnail_image_provider.cpp
    thumbnail_image_provider.hpp
)

# consider https://doc.qt.io/QtQuickCompiler/ in case of load performance problems
file(GLOB_RECURSE
    QML_RESOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml *.js
)

qt6_add_qml_module(quick_items
    URI "quick_items"
    VERSION 1.0
    SOURCES ${QUICK_ITEMS_CPP_SOURCES}
    QML_FILES ${QML_RESOURCES}
    STATIC
    RESOURCE_PREFIX /photo_broom
)

target_link_libraries(quick_items
    PUBLIC
        core
        face_recognition
        gui_models
        project_utils

        Qt::Quick
        Qt::Widgets                  # TODO: remove ASAP
)

target_include_directories(quick_items
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)
