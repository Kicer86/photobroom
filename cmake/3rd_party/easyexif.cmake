
# script for easyexif download & build
# ExternalProject module could be used here, but it has serious drawback: 
# stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

include(ExternalProject)

function(addEasyExif)

    find_program(SVN_EXECUTABLE svn DOC "Path to subversion executable")

    if (SVN_EXECUTABLE)
    
        #create dir rule
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/src
                           COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/3rd_party/src
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
                           
        #download easyexif rule
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp
                           COMMAND ${SVN_EXECUTABLE} checkout http://easyexif.googlecode.com/svn/trunk/ easyexif
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src
                           DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/src)

        add_library(easyexif STATIC ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp)

        set_source_files_properties(${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp PROPERTIES GENERATED TRUE)
    endif()

endfunction(addEasyExif)
