
find_package(Qt5Widgets REQUIRED)

include(GenerateExportHeader)

add_library(KWidgetsAddons
                kwidgetsaddons/src/kcolorcombo.cpp    kwidgetsaddons/src/kcolorcombo.h
                kwidgetsaddons/src/kratingpainter.cpp kwidgetsaddons/src/kratingpainter.h
                kwidgetsaddons/src/kratingwidget.cpp  kwidgetsaddons/src/kratingwidget.h
)

target_include_directories(KWidgetsAddons
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
                            INTERFACE
                                kwidgetsaddons/src
)

target_link_libraries(KWidgetsAddons
                        PUBLIC
                            Qt5::Widgets
)

set_target_properties(KWidgetsAddons PROPERTIES AUTOMOC TRUE)

set(KWidgetsAddonsMacros
    "
    #define KWIDGETSADDONS_ENABLE_DEPRECATED_SINCE(x,y) 0
    #define KWIDGETSADDONS_BUILD_DEPRECATED_SINCE(x,y) 0
    "
)

generate_export_header(KWidgetsAddons
                       CUSTOM_CONTENT_FROM_VARIABLE KWidgetsAddonsMacros)
