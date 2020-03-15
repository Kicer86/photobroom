
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
                            core
                            Qt5::Quick
)

target_include_directories(quick_views
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(quick_views PROPERTIES AUTOMOC TRUE)
generate_export_header(quick_views)
