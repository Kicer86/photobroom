
find_package(Qt5Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})

add_executable(config_generator main.cpp)
target_link_libraries(config_generator Qt5::Core)
