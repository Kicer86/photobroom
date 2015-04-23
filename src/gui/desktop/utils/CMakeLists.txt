

include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>)

set(UTILS_SOURCES
    variant_display.cpp
    )

set(UTILS_HEADERS
    variant_display.hpp
    )

add_library(gui_utils OBJECT ${UTILS_SOURCES} ${UTILS_WRAPPED_CPP} ${UTILS_HEADERS} ${UTILS_WRAPPED_UI})
set_target_properties(gui_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
