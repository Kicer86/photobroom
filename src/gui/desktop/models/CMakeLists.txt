
include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>
                   )

set(SRC
    list_model.cpp
    )

set(HEADERS
    list_model.hpp
    )

set(TO_MOC

)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_models OBJECT ${SRC} ${MOCED} ${HEADERS})
set_target_properties(gui_models PROPERTIES POSITION_INDEPENDENT_CODE ON)
