
project(third_party_libraries)

cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

set(INSTALL_LOCATION ${PROJECT_BINARY_DIR}/install)
set(COMMON_OPTIONS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION} 
                   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                   -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM})
                   
option(BUILD_EASYEXIF    "Build easyexif.")
option(BUILD_HASHLIBRARY "Build hashlibrary.")
option(BUILD_JSONCPP     "Build jsoncpp.")
option(BUILD_OPENLIBRARY "Build openlibrary.")
 
       
# EASYEXIF
if(BUILD_EASYEXIF)  
    ExternalProject_Add(easyexif
        GIT_REPOSITORY https://github.com/mayanklahiri/easyexif.git
        GIT_TAG v1.0
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/easyexif/CMakeLists.txt ${PROJECT_BINARY_DIR}/src/easyexif && ${GIT_EXECUTABLE} add -A ${PROJECT_BINARY_DIR}/src/easyexif
        PREFIX ${PROJECT_BINARY_DIR} 
        CMAKE_ARGS ${COMMON_OPTIONS}
        )
endif()


# HASH-LIBRARY
if(BUILD_HASHLIBRARY)
    ExternalProject_Add(hash-library
        GIT_REPOSITORY http://create.stephan-brumme.com/hash-library/.git
        GIT_TAG 21d2fed433dcb05a9a89561ca8774ef91fe48ffc
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/hash-library/CMakeLists.txt ${PROJECT_BINARY_DIR}/src/hash-library && ${GIT_EXECUTABLE} add -A ${PROJECT_BINARY_DIR}/src/hash-library
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
        )
endif()
       
       
# JSONCPP
if(BUILD_JSONCPP)
    ExternalProject_Add(jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG 9234cbbc90d1f6c70dd5a90b4d533779e45c820c
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
        )
endif()


# OPEN LIBRARY
if(BUILD_OPENLIBRARY)
    ExternalProject_Add(openlibrary
        GIT_REPOSITORY https://github.com/Kicer86/openlibrary.git
        GIT_TAG v_2.0
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS ${COMMON_OPTIONS}
        )
endif()
