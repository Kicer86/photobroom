

include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>
                   )


set(UTILS_SOURCES
    config_tools.cpp
    path_checker.cpp
    photos_collector.cpp
    signal_filter.cpp
    thread_multiplexer.cpp
    variant_display.cpp
    )

set(UTILS_HEADERS
    config_tools.hpp
    path_checker.hpp
    photos_collector.hpp
    signal_filter.hpp
    thread_multiplexer.hpp
    variant_display.hpp
)


set(UTILS_FILES_TO_MOC
    path_checker.hpp
    photos_collector.hpp
    signal_filter.hpp
    thread_multiplexer.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(UTILS_WRAPPED_CPP ${UTILS_FILES_TO_MOC})

add_library(gui_utils OBJECT ${UTILS_SOURCES} ${UTILS_WRAPPED_CPP} ${UTILS_HEADERS})
set_target_properties(gui_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
