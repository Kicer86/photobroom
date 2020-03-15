
find_package(Qt5Quick REQUIRED)

qt5_add_resources(QML_RESOURCES
                    resources.qrc
)


add_library(quick_views OBJECT
                ${QML_RESOURCES}
                photo_item.cpp
                photo_item.hpp
)


target_link_libraries(quick_views
                        PUBLIC
                            Qt5::Quick
)
