
function(addJsonCpp)

    add_library(jsoncpp STATIC ${CMAKE_SOURCE_DIR}/3rd_party/jsoncpp/dist/jsoncpp.cpp)
    set_target_properties(jsoncpp PROPERTIES POSITION_INDEPENDENT_CODE ON)

endfunction(addJsonCpp)
