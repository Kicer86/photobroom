 
#script for Nettle download & build

include(ExternalProject)

function(addNettle)
  
    ExternalProject_Add(Nettle 
                        PREFIX ${CMAKE_BINARY_DIR}/3rd_party
                        URL https://ftp.gnu.org/gnu/nettle/nettle-3.0.tar.gz
                        CONFIGURE_COMMAND ../Nettle/configure --prefix=${CMAKE_BINARY_DIR}/3rd_party/root
                        INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd_party/root)

endfunction(addNettle)
