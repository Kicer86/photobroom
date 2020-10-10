
find_package(Qt5 REQUIRED COMPONENTS Gui Svg)

add_executable(img_manip EXCLUDE_FROM_ALL img_manip.cpp)
target_link_libraries(img_manip Qt::Gui Qt::Svg)
