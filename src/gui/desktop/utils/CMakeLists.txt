

include_directories($<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>
                   )


set(UTILS_SOURCES
    config_tools.cpp
    path_checker.cpp
    photos_collector.cpp
    signal_filter.cpp
    tag_value_model.cpp
    thumbnail_acquisitor.cpp
    thumbnail_generator.cpp
    variant_display.cpp
    )

set(UTILS_HEADERS
    config_tools.hpp
    ithumbnail_acquisitor.hpp
    ithumbnail_generator.hpp
    path_checker.hpp
    photos_collector.hpp
    signal_filter.hpp
    tag_value_model.hpp
    thumbnail_acquisitor.hpp
    thumbnail_generator.hpp
    variant_display.hpp
)


set(UTILS_FILES_TO_MOC
    path_checker.hpp
    photos_collector.hpp
    signal_filter.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(UTILS_WRAPPED_CPP ${UTILS_FILES_TO_MOC})

add_library(gui_utils OBJECT ${UTILS_SOURCES} ${UTILS_WRAPPED_CPP} ${UTILS_HEADERS})
set_target_properties(gui_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
