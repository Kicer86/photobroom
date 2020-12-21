
set(DLIB_LIBRARIES)

if(TARGET CONAN_PKG::dlib)
    set(DLIB_LIBRARIES CONAN_PKG::dlib)

    message("Using dlib from conan")
else()
    find_package(dlib REQUIRED ONLY_CMAKE_FIND_ROOT_PATH)
    find_package(Threads REQUIRED)
    set(DLIB_LIBRARIES dlib::dlib)

    message("Using system dlib")
endif()


find_package_handle_standard_args(Dlib DEFAULT_MSG DLIB_LIBRARIES)
