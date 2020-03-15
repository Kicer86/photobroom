
find_package(Qt5Quick REQUIRED)

qt5_add_resources(QML_RESOURCES
                    resources.qrc
)


add_library(qml_resources OBJECT
                ${QML_RESOURCES}
                photo_item.cpp
                photo_item.hpp
)


target_link_libraries(qml_resources
                        PUBLIC
                            Qt5::Quick
)
