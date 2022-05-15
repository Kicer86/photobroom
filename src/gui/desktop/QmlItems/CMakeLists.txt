
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Quick
)

set(CMAKE_AUTOMOC ON)

# consider https://doc.qt.io/QtQuickCompiler/ in case of load performance problems
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
    STATIC
    RESOURCE_PREFIX /photo_broom
    ${EXTRA_QML_OPTIONS}
)

#qt6_target_compile_qml_to_cpp(qml_items
#    QML_FILES ${QML_SOURCES}
#)

target_link_libraries(qml_items
    PUBLIC
        Qt::Quick

        #Qt::QmlPrivate
        #Qt::QuickPrivate
)
