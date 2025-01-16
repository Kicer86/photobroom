
find_package(Qt6 REQUIRED COMPONENTS
    Quick
)

set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE
    QML_VIEWS_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    *.qml
)

qt6_add_qml_module(quick_items_views
    URI "quick_items.views"
    VERSION 1.0
    QML_FILES ${QML_VIEWS_SOURCES}
    STATIC
    RESOURCE_PREFIX /photo_broom
)
