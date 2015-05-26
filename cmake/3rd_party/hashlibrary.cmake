
function(addHashLibrary)

    add_library(hashlibrary STATIC ${CMAKE_SOURCE_DIR}/3rd_party/hash-library/sha256.cpp)
    set_target_properties(hashlibrary PROPERTIES POSITION_INDEPENDENT_CODE ON)

endfunction(addHashLibrary)
