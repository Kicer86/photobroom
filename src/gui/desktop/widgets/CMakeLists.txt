
find_package(Qt6 REQUIRED COMPONENTS QuickWidgets Widgets)

set(SRCS
    media_preview.cpp
    project_creator/project_creator_dialog.cpp
    qtext_choosefile.cpp
)

set(HEADERS
    media_preview.hpp
    project_creator/project_creator_dialog.hpp
    qtext_choosefile.hpp
)

#set of file to be moced
set(TO_MOC
    media_preview.hpp
    project_creator/project_creator_dialog.hpp
    qtext_choosefile.hpp
)

qt6_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_widgets OBJECT ${SRCS} ${MOCED} ${HEADERS})
set_target_properties(gui_widgets PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_widgets
                        PRIVATE
                            core
                            database
                            face_recognition
                            photos_crawler
                            project_utils
                            quick_items
                            system
                            Qt::Widgets
                            Qt::QuickWidgets
)

target_include_directories(gui_widgets
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
)
