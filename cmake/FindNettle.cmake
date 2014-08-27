

#rules for finding Nettle

if(TARGET Nettle)

    set(NETTLE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/3rd_party/src/)
    set(NETTLE_LIBRARY nettle)

else()

    find_path(NETTLE_INCLUDE_DIR nettle/nettle.h
                                 PATHS ${CMAKE_BINARY_DIR}/3rd_party/root/include)

    find_library(NETTLE_LIBRARY NAMES nettle
                                PATHS ${CMAKE_BINARY_DIR}/3rd_party/root/lib)

endif()

set(NETTLE_LIBRARIES ${NETTLE_LIBRARY} )
set(NETTLE_INCLUDE_DIRS ${NETTLE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Nettle DEFAULT_MSG NETTLE_LIBRARY NETTLE_INCLUDE_DIR)

mark_as_advanced(NETTLE_INCLUDE_DIR NETTLE_LIBRARY)
