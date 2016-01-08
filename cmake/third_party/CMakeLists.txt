
project(third_party_libraries)

cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

set(INSTALL_LOCATION ${PROJECT_BINARY_DIR}/install)

option(BUILD_KCRASH      "Build KCrash will all dependencies.")
option(BUILD_EASYEXIF    "Build easyexif.")
option(BUILD_HASHLIBRARY "Build hashlibrary.")
option(BUILD_JSONCPP     "Build jsoncpp.")


# KDE5
if(BUILD_KCRASH)

    ExternalProject_Add(extra-cmake-modules
        GIT_REPOSITORY git://anongit.kde.org/extra-cmake-modules.git
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
        )

    ExternalProject_Add(kcoreaddons
        DEPENDS extra-cmake-modules
        GIT_REPOSITORY git://anongit.kde.org/kcoreaddons.git
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
                -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
        )

    ExternalProject_Add(kwindowsystem
        DEPENDS extra-cmake-modules
        GIT_REPOSITORY git://anongit.kde.org/kwindowsystem.git
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
                -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
        )
        
    ExternalProject_Add(kcrash
        DEPENDS extra-cmake-modules
                kcoreaddons
                kwindowsystem
        GIT_REPOSITORY git://anongit.kde.org/kcrash.git
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
                -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
                -DKF5CoreAddons_DIR=${INSTALL_LOCATION}/lib/cmake/KF5CoreAddons
                -DKF5WindowSystem_DIR=${INSTALL_LOCATION}/lib/cmake/KF5WindowSystem
        )
endif()
       
       
# EASYEXIF
if(BUILD_EASYEXIF)  
    ExternalProject_Add(easyexif
        GIT_REPOSITORY https://github.com/mayanklahiri/easyexif.git
        GIT_TAG v1.0
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/easyexif/CMakeLists.txt ${PROJECT_BINARY_DIR}/src/easyexif && ${GIT_EXECUTABLE} add -A ${PROJECT_BINARY_DIR}/src/easyexif
        PREFIX ${PROJECT_BINARY_DIR} 
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
        )
endif()


# HASH-LIBRARY
if(BUILD_HASHLIBRARY)
    ExternalProject_Add(hash-library
        GIT_REPOSITORY http://create.stephan-brumme.com/hash-library/.git
        GIT_TAG 21d2fed433dcb05a9a89561ca8774ef91fe48ffc
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/hash-library/CMakeLists.txt ${PROJECT_BINARY_DIR}/src/hash-library && ${GIT_EXECUTABLE} add -A ${PROJECT_BINARY_DIR}/src/hash-library
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
        )
endif()
       
       
# JSONCPP
if(BUILD_JSONCPP)
    ExternalProject_Add(jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG 9234cbbc90d1f6c70dd5a90b4d533779e45c820c
        PREFIX ${PROJECT_BINARY_DIR}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
        )
endif()
