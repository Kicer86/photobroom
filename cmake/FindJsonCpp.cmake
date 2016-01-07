
#rules for finding JsonCpp
find_path(JSONCPP_INCLUDE_DIR json/json.h
          HINTS /usr/include/jsoncpp                  #debian
         )
find_library(JSONCPP_LIBRARY NAMES jsoncpp)
    
set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY} )
set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JsonCpp DEFAULT_MSG JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR)

mark_as_advanced(JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)
