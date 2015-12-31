
#rules for finding JsonCpp

option(USE_LOCAL_JSONCPP "Use jsoncpp library from 3rd_party directory when ON. Otherwise try to find it.")

if(USE_LOCAL_JSONCPP)

    include(${PROJECT_SOURCE_DIR}/cmake/3rd_party/jsoncpp.cmake)
    addJsonCpp()

    set(JSONCPP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd_party/jsoncpp/include)
    set(JSONCPP_LIBRARY jsoncpp_lib_static)

else()

    find_path(JSONCPP_INCLUDE_DIR json/json.h
              HINTS /usr/include/jsoncpp                  #debian
             )
    find_library(JSONCPP_LIBRARY NAMES jsoncpp)
    
endif()

set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY} )
set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JsonCpp DEFAULT_MSG JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR)

mark_as_advanced(JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)
