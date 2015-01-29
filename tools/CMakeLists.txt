
if(WIN32)
    configure_file(PhotoBroom.hpp.cmake PhotoBroom.hpp @ONLY)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    add_executable(PhotoBroom WIN32 PhotoBroom.cpp PhotoBroom.rc)
    add_dependencies(PhotoBroom Images)

    install(TARGETS PhotoBroom RUNTIME DESTINATION ".")
endif()

find_package(Qt5Gui REQUIRED)
find_package(Qt5Svg REQUIRED)

include_directories(SYSTEM ${Qt5Gui_INCLUDE_DIRS})

add_executable(img_manip img_manip.cpp)
target_link_libraries(img_manip Qt5::Gui Qt5::Svg)
