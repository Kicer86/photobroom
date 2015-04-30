
function(addEasyExif)

    add_library(easyexif STATIC ${CMAKE_SOURCE_DIR}/3rd_party/easyexif/exif.cpp)

    if(MSVC)

    else()
        set_target_properties(easyexif PROPERTIES COMPILE_FLAGS -fPIC)    #use fPIC as easyexif will be linked against dynamic objects
    endif()

endfunction(addEasyExif)
