
#rules for finding HashLibrary

if(EXISTS ${CMAKE_SOURCE_DIR}/3rd_party/hash-library)

    include(${PROJECT_SOURCE_DIR}/cmake/3rd_party/hashlibrary.cmake)
    addHashLibrary()

    set(HASHLIBRARY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd_party/)
    set(HASHLIBRARY_LIBRARY hashlibrary)

else()

    find_path(HASHLIBRARY_INCLUDE_DIR hash-library/sha256.h)
    find_library(HASHLIBRARY_LIBRARY NAMES hashlibrary)

endif()

set(HASHLIBRARY_LIBRARIES ${HASHLIBRARY_LIBRARY} )
set(HASHLIBRARY_INCLUDE_DIRS ${HASHLIBRARY_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HashLibrary DEFAULT_MSG HASHLIBRARY_LIBRARY HASHLIBRARY_INCLUDE_DIR)

mark_as_advanced(HASHLIBRARY_INCLUDE_DIR HASHLIBRARY_LIBRARY)
