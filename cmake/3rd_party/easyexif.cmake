
# script for easyexif download & build
# ExternalProject module could be used here, but it has serious drawback: 
# stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage

include(ExternalProject)

function(addEasyExif)

    find_package(Git)

    if(GIT_FOUND)
                              
        #download easyexif rule
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif_downloaded
                           COMMAND ${GIT_EXECUTABLE} clone https://github.com/mayanklahiri/easyexif.git
                           COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif_downloaded
                           DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/src/dir
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src)

        set_source_files_properties(${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp PROPERTIES GENERATED TRUE)
        
        add_custom_target(download_easy_exif
                          DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif_downloaded)
        
        add_library(easyexif STATIC ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/exif.cpp)
        add_dependencies(easyexif download_easy_exif)
        
        if(MSVC)
        
        else()
            set_target_properties(easyexif PROPERTIES COMPILE_FLAGS -fPIC)    #use fPIC as easyexif will be linked against dynamic objects
        endif()
    else()
        message(FATAL_ERROR "EasyEXIF cannot be downloaded. No git command found.")
    endif()

endfunction(addEasyExif)
