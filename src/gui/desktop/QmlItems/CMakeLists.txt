
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Quick
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
    *.qml
)

if(${Qt6Quick_VERSION} EQUAL "6.3.0")
    list(APPEND EXTRA_QML_OPTIONS NO_CACHEGEN)              # disable qmlsc due to a bug https://bugreports.qt.io/browse/QTBUG-99042
endif()

if(${Qt6Quick_VERSION} VERSION_GREATER_EQUAL "6.4")
    list(APPEND EXTRA_QML_OPTIONS ENABLE_TYPE_COMPILER)     # compile qml files
endif()

qt6_add_qml_module(qml_items
    URI "QmlItems"
    VERSION 1.0
    QML_FILES ${QML_SOURCES}
    SOURCES ${CPP_SOURCES}
    STATIC
    RESOURCE_PREFIX /photo_broom
    ${EXTRA_QML_OPTIONS}
)

target_link_libraries(qml_items
    PUBLIC
        Qt::Quick
)

if(${Qt6Quick_VERSION} VERSION_GREATER_EQUAL "6.4")
    target_link_libraries(qml_items
        PRIVATE
            Qt::QmlPrivate
            Qt::QuickPrivate
    )
endif()
