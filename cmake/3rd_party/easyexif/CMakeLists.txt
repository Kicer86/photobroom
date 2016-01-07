
project(easyexif)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

add_library(easyexif STATIC exif.cpp)       
set_target_properties(easyexif PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS easyexif DESTINATION lib)
install(FILES exif.h DESTINATION include/easyexif)
