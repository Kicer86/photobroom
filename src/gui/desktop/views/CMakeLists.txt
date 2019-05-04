
set(SRC
    view_impl/data.cpp
    view_impl/model_index_info.cpp
    view_impl/positions_calculator.cpp
    view_impl/positions_reseter.cpp
    view_impl/positions_translator.cpp
    images_tree_view.cpp
    tree_item_delegate.cpp
    )

set(HEADERS
    view_impl/data.hpp
    view_impl/model_index_info.hpp
    view_impl/positions_calculator.hpp
    view_impl/positions_reseter.hpp
    view_impl/positions_translator.hpp
    view_impl/view_data_set.hpp
    images_tree_view.hpp
    tree_item_delegate.hpp
)

set(TO_MOC
    images_tree_view.hpp
    tree_item_delegate.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_views OBJECT ${SRC} ${MOCED} ${HEADERS})
set_target_properties(gui_views PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_views
                        PRIVATE
                            core
                            database
                            photos_crawler
                            Qt5::Widgets
)

target_include_directories(gui_views
                            SYSTEM PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
)
