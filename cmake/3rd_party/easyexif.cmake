
# script for easyexif download & build
# ExternalProject module could be used here, but it has serious drawback: 
# stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

include(ExternalProject)

function(addEasyExif)

    find_program(SVN_EXECUTABLE svn DOC "Path to subversion executable")

    if (SVN_EXECUTABLE)
                              
        #download easyexif rule
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp
                           COMMAND ${SVN_EXECUTABLE} checkout http://easyexif.googlecode.com/svn/trunk/ easyexif
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src)

        add_library(easyexif STATIC ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp)
        
        if(MSVC)
        
        else()
            set_source_files_properties(${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp PROPERTIES COMPILE_FLAGS -fPIC)    #use fPIC as easyexif will be linked against dynamic objects
        endif()

        set_source_files_properties(${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp PROPERTIES GENERATED TRUE)
    endif()

endfunction(addEasyExif)
