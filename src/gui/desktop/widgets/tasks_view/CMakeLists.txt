



set(SRCS
    tasks_view_widget.cpp
    )

set(HEADERS
    tasks_view_widget.hpp
    )

#set of file to be moced
set(TO_MOC

    )


qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_widgets_tasks_view OBJECT ${SRCS} ${MOCED} ${HEADERS})
set_target_properties(gui_widgets_tasks_view PROPERTIES POSITION_INDEPENDENT_CODE ON)
