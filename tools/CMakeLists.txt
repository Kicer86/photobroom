
find_package(Qt5Gui REQUIRED)
find_package(Qt5Svg REQUIRED)

include_directories(SYSTEM ${Qt5Gui_INCLUDE_DIRS})

add_executable(img_manip img_manip.cpp)
target_link_libraries(img_manip Qt5::Gui Qt5::Svg)
