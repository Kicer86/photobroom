
include_directories(
                    $<TARGET_PROPERTY:configuration,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>
)

set(SRC
    view_impl/data.cpp
    view_impl/model_index_info.cpp
    view_impl/positions_calculator.cpp
    view_impl/positions_reseter.cpp
    view_impl/positions_translator.cpp
    view_impl/view_data_set.cpp
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
    view_impl/view_data_set.hpp
    tree_item_delegate.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_views OBJECT ${SRC} ${MOCED} ${HEADERS})
set_target_properties(gui_views PROPERTIES POSITION_INDEPENDENT_CODE ON)
