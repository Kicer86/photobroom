
if(TARGET CONAN_PKG::dlib)
    add_library(dlib ALIAS CONAN_PKG::dlib)

    message("Using dlib from conan")
else()
    find_package(dlib REQUIRED ONLY_CMAKE_FIND_ROOT_PATH)
    find_package(Threads REQUIRED)
    add_library(dlib ALIAS dlib::dlib)

    message("Using system dlib")
endif()

set(DLIB_LIBRARIES dlib)

find_package_handle_standard_args(Dlib DEFAULT_MSG DLIB_LIBRARIES)
