
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickWidgets REQUIRED)

qt5_add_resources(QML_RESOURCES
                    resources.qrc
)

qt5_wrap_cpp(MOCED
                photo.hpp
                qml_setup.hpp
)


add_library(quick_views OBJECT
                ${QML_RESOURCES}
                ${MOCED}
                photo.cpp
                photo.hpp
                qml_setup.cpp
                qml_setup.hpp
                qml_utils.cpp
                qml_utils.hpp
)


target_link_libraries(quick_views
                        PUBLIC
                            core
                            Qt5::Quick
                            Qt5::QuickWidgets
)

target_include_directories(quick_views
                                PUBLIC
                                    ${CMAKE_CURRENT_BINARY_DIR}
)

generate_export_header(quick_views)
