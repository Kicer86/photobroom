 
#script for Nettle download & build

function(addNettle)
  
    find_program(WGET_EXECUTABLE wget DOC "Path to wget executable")

    if (WGET_EXECUTABLE)    
                                 
        #download Nettle rule
        add_custom_command(OUTPUT  ${CMAKE_BINARY_DIR}/3rd_party/src/nettle-3.0.tar.gz
                           COMMAND ${WGET_EXECUTABLE} https://ftp.gnu.org/gnu/nettle/nettle-3.0.tar.gz
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src)
                           
        #unpack Nettle rule
        add_custom_command(OUTPUT  ${CMAKE_BINARY_DIR}/3rd_party/src/nettle-3.0/configure
                           COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_BINARY_DIR}/3rd_party/src/nettle-3.0.tar.gz
                           DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/src/nettle-3.0.tar.gz
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src/)
                           
        #build Nettle rule
        add_custom_command(OUTPUT  ${CMAKE_BINARY_DIR}/3rd_party/root/share/info/nettle.info
                           COMMAND ./configure --prefix=${CMAKE_BINARY_DIR}/3rd_party/root
                           COMMAND make
                           COMMAND make install
                           DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/src/nettle-3.0/configure
                           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd_party/src/nettle-3.0)

        add_custom_target(nettle
                          DEPENDS ${CMAKE_BINARY_DIR}/3rd_party/root/share/info/nettle.info)
                                  
    endif()

endfunction(addNettle)
