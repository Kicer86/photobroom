
qt5_add_resources(QML_RESOURCES
                    resources.qrc
)


add_library(qml_resources OBJECT
            ${QML_RESOURCES}
)
