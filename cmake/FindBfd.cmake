
#rules for finding Bfd
find_path(BFD_INCLUDE_DIR bfd.h)
find_library(BFD_LIBRARY NAMES bfd)
    
set(BFD_LIBRARIES ${BFD_LIBRARY} )
set(BFD_INCLUDE_DIRS ${BFD_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Bfd DEFAULT_MSG BFD_LIBRARY BFD_INCLUDE_DIR)

mark_as_advanced(BFD_INCLUDE_DIR BFD_LIBRARY)
