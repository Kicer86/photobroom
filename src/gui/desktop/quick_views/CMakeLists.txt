
find_package(Qt5Quick REQUIRED)

qt5_add_resources(QML_RESOURCES
                    resources.qrc
)


add_library(quick_views OBJECT
                ${QML_RESOURCES}
                photo.cpp
                photo.hpp
                qml_setup.cpp
                qml_setup.hpp
)


target_link_libraries(quick_views
                        PUBLIC
                            Qt5::Quick
)

target_include_directories(quick_views
                                PUBLIC
                                    ${PROJECT_BINARY_DIR}
                                    ${CMAKE_CURRENT_BINARY_DIR}
)

generate_export_header(quick_views)
