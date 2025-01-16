
find_package(Qt6 REQUIRED COMPONENTS
    Quick
)

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE
    QML_COMPONENTS_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    *.qml *.js
)

set_source_files_properties(Constants.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt6_add_qml_module(quick_items_components
    URI "quick_items.components"
    VERSION 1.0
    QML_FILES ${QML_COMPONENTS_SOURCES}
    STATIC
    RESOURCE_PREFIX /photo_broom
)
