
if(WIN32)
    add_executable(PhotoBroom WIN32 main.cpp)

    install(TARGETS PhotoBroom RUNTIME DESTINATION ".")
endif()

find_package(Qt5Gui REQUIRED)
include_directories(SYSTEM ${Qt5Gui_INCLUDE_DIRS})
add_executable(img_manip img_manip.cpp)
target_link_libraries(img_manip Qt5::Gui)
