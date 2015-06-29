
#rules for finding JsonCpp

if(EXISTS ${CMAKE_SOURCE_DIR}/3rd_party/jsoncpp)

    include(${PROJECT_SOURCE_DIR}/cmake/3rd_party/jsoncpp.cmake)
    addJsonCpp()

    set(JSONCPP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rd_party/)
    set(JSONCPP_LIBRARY jsoncpp)

else()

    find_path(JSONCPP_INCLUDE_DIR jsoncpp/json.h)
    find_library(JSONCPP_LIBRARY NAMES jsoncpp)
    
endif()

set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY} )
set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JsonCpp DEFAULT_MSG JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR)

mark_as_advanced(JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)
