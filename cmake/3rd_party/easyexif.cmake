
function(addEasyExif)

    add_library(easyexif STATIC ${CMAKE_SOURCE_DIR}/3rd_party/easyexif/exif.cpp)       
    set_target_properties(easyexif PROPERTIES POSITION_INDEPENDENT_CODE ON)

endfunction(addEasyExif)
