 
find_package(OpenSSL)
find_package(Nettle)
find_package(OpenLibrary 2.0 REQUIRED)

include(GenerateExportHeader)
include(${OPENLIBRARY_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(OPENSSL_FOUND)

    set(CRYPTOGRAPHIC_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR})
    set(CRYPTOGRAPHIC_LIBRARIES ${OPENSSL_LIBRARIES})
    install(FILES ${CMAKE_SOURCE_DIR}/docs/install/OpenSSL.txt DESTINATION ${PATH_DOCS})

    set(SRCS openssl_adapter.cpp)

elseif(NETTLE_FOUND)

    set(CRYPTOGRAPHIC_INCLUDE_DIR ${NETTLE_INCLUDE_DIRS})
    set(CRYPTOGRAPHIC_LIBRARIES ${NETTLE_LIBRARIES})

    set(SRCS nettle_adapter.cpp)

endif()

if(NOT SRCS)
    message(FATAL_ERROR "Could not find any crypto lib")
endif()

set(SRCS ${SRCS} utils.cpp utils.hpp)

include_directories(SYSTEM ${CRYPTOGRAPHIC_INCLUDE_DIR})

add_library(crypto ${SRCS} crypto.hpp)
target_link_libraries(crypto PRIVATE ${CRYPTOGRAPHIC_LIBRARIES})
target_include_directories(crypto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(crypto PROPERTIES POSITION_INDEPENDENT_CODE ON)
generate_export_header(crypto)
hideSymbols(crypto)
