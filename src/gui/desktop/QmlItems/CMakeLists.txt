
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Quick
)

set(CMAKE_AUTOMOC ON)

set(CPP_SOURCES
    picture_item.cpp
    picture_item.hpp
)

file(GLOB_RECURSE
    QML_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml
)

if(${Qt6Quick_VERSION} EQUAL "6.3.0")
    set(EXTRA_QML_OPTIONS NO_CACHEGEN)      # disable qmlsc due to a bug https://bugreports.qt.io/browse/QTBUG-99042
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

if(${Qt6Quick_VERSION} VERSION_GREATER "6.3.1")     # TODO: in Qt 6.3.0 there is an error: 'qtquickglobal_p.h': No such file or directory
    qt6_target_compile_qml_to_cpp(qml_items
        QML_FILES ${QML_SOURCES}
    )

    target_link_libraries(qml_items
        PRIVATE
            Qt::QmlPrivate
            Qt::QuickPrivate
    )
endif()
