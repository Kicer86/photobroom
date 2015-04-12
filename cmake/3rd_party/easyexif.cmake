
# script for easyexif download & build
# ExternalProject module could be used here, but it has serious drawback: 
# stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

include(ExternalProject)

function(addEasyExif)

    find_package(git)

    if(GIT_FOUND)
                              
        #download easyexif rule
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp
                           COMMAND ${GIT_EXECUTABLE} clone https://github.com/mayanklahiri/easyexif.git
                           DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/src/dir
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src)

        add_library(easyexif STATIC ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp)
        
        if(MSVC)
        
        else()
            set_target_properties(easyexif PROPERTIES COMPILE_FLAGS -fPIC)    #use fPIC as easyexif will be linked against dynamic objects
        endif()

        set_source_files_properties(${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp PROPERTIES GENERATED TRUE)
    else()
        message(FATAL_ERROR "EasyEXIF cannot be downloaded. No git command found.")
    endif()

endfunction(addEasyExif)
