
find_package(Qt6 6.4 REQUIRED COMPONENTS
    Quick
)

find_package(Qt6 6.4  COMPONENTS
    QmlPrivate
    QuickPrivate
)

set(CMAKE_AUTOMOC ON)

set(CPP_SOURCES
    file_info.cpp
    file_info.hpp
    picture_item.cpp
    picture_item.hpp
    utils.cpp
    utils.hpp
)

file(GLOB_RECURSE
    QML_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    *.qml
)

if(${Qt6Quick_VERSION} VERSION_GREATER "6.4.2")             # // TODO: >= 6.4 should probably be enought, but there are issues with this on ubuntu on CI. Probably caused by some missing packages. To be figured out later
    list(APPEND EXTRA_QML_OPTIONS ENABLE_TYPE_COMPILER)     # compile qml files
endif()

qt6_add_qml_module(qml_items
    URI "QmlItems"
    VERSION 1.0
    QML_FILES ${QML_SOURCES}
    SOURCES ${CPP_SOURCES}
    STATIC
    RESOURCE_PREFIX /photo_broom
    ${EXTRA_QML_OPTIONS}                                    # compile qml files
)

target_link_libraries(qml_items
    PUBLIC
        Qt::Quick
    PRIVATE
        Qt::QmlPrivate
        Qt::QuickPrivate
)
