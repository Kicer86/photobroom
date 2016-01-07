
project(easyexif)

cmake_minimum_required(VERSION 3.0)

# trick to fix build error on gcc
add_definitions(-D_MSC_VER)

add_library(hashlibrary STATIC sha256.cpp)       
set_target_properties(hashlibrary PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS hashlibrary DESTINATION lib)
install(FILES sha256.h DESTINATION include/hashlibrary)
