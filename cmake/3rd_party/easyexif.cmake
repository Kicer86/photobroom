
#script for easyexif download & build

include(ExternalProject)

function(addEasyExif)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/3rd_party/easyexif.cmakelists ${CMAKE_BINARY_DIR}/3rd_party/src/easyexif/CMakeLists.txt)

    ExternalProject_Add(easyexif
                        PREFIX ${CMAKE_BINARY_DIR}/3rd_party
                        SVN_REPOSITORY http://easyexif.googlecode.com/svn/trunk/
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/3rd_party/root
                        INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd_party/root)

endfunction(addEasyExif)
